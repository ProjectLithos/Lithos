[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$ProjectRoot,
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [ValidateSet('Kernel', 'Desktop')][string]$ProjectType = 'Kernel',
    [switch]$Clean
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Find-LlvmTool {
    param([string]$Name)
    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }

    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path $vswhere) {
        $installation = (& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Llvm.Clang -property installationPath | Out-String).Trim()
        if ($installation) {
            foreach ($relative in @("VC\Tools\Llvm\x64\bin\$Name.exe", "VC\Tools\Llvm\bin\$Name.exe")) {
                $candidate = Join-Path $installation $relative
                if (Test-Path $candidate) { return $candidate }
            }
        }
    }
    throw "LLVM tool '$Name' was not found. Add the Visual Studio Clang/LLVM component."
}

$sdkRoot = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'Machine')
if ([string]::IsNullOrWhiteSpace($sdkRoot)) { $sdkRoot = $env:OESDK_ROOT }
if ([string]::IsNullOrWhiteSpace($sdkRoot) -or -not (Test-Path $sdkRoot)) {
    throw 'OESDK_ROOT is not set. Repair or reinstall OESDK.'
}

$rawProjectRoot = $ProjectRoot.Trim()
# OESDK 0.0.8 templates quoted $(ProjectDir), whose trailing backslash could
# escape the closing quote in the Windows command line. Accept that malformed
# argument so projects already created with 0.0.8 continue to build.
$embeddedQuote = $rawProjectRoot.IndexOf('"')
if ($embeddedQuote -ge 0) {
    $trailingArguments = $rawProjectRoot.Substring($embeddedQuote + 1)
    if ($trailingArguments -match '-Configuration\s+[^A-Za-z]*(Debug|Release)') {
        $Configuration = $Matches[1]
    }
    if ($trailingArguments -match '-ProjectType\s+[^A-Za-z]*(Kernel|Desktop)') {
        $ProjectType = $Matches[1]
    }
    if ($trailingArguments -match '(?:^|\s)-Clean(?:\s|$)') {
        $Clean = $true
    }
    $rawProjectRoot = $rawProjectRoot.Substring(0, $embeddedQuote).Trim()
}
if ([string]::IsNullOrWhiteSpace($rawProjectRoot)) {
    throw 'The Visual Studio project directory is empty or malformed.'
}
$projectRoot = [IO.Path]::GetFullPath($rawProjectRoot)
$outputRoot = Join-Path $projectRoot ("Build\" + $Configuration)
if ($Clean) {
    if (Test-Path $outputRoot) { Remove-Item -LiteralPath $outputRoot -Recurse -Force }
    Write-Host "Cleaned $outputRoot"
    exit 0
}

$objectRoot = Join-Path $outputRoot 'Objects'
[void][IO.Directory]::CreateDirectory($objectRoot)
$clang = Find-LlvmTool 'clang'
$archiver = Find-LlvmTool 'llvm-ar'
$linker = Find-LlvmTool 'ld.lld'
$includeRoot = Join-Path $sdkRoot 'Include'
$bootSource = Join-Path $sdkRoot 'Architecture\x86_64\boot.S'
$linkerScript = Join-Path $sdkRoot 'Architecture\x86_64\linker.ld'

$definitions = @()
$optimization = '-O2'
if ($Configuration -eq 'Debug') {
    $definitions += '-DDEBUG=1'
    $optimization = '-O0'
}
if ($ProjectType -eq 'Desktop') { $definitions += '-DOESDK_DESKTOP=1' }

$common = @(
    '--target=x86_64-unknown-none-elf', '-ffreestanding', '-fno-builtin',
    '-fno-stack-protector', '-fno-pic', '-fno-pie', '-mno-red-zone',
    '-mcmodel=kernel', '-Wall', '-Wextra', '-Wpedantic', $optimization,
    '-g', '-isystem', $includeRoot
) + $definitions

$sdkSourceRoot = Join-Path $sdkRoot 'Source'
$libcSourceRoot = Join-Path $sdkSourceRoot 'LibC'
$projectSourceRoot = Join-Path $projectRoot 'Source'

# LibC is a separate Newlib target library. It must not be compiled into every
# freestanding kernel because its port sources require the built Newlib target
# headers and runtime. Kernel builds use only the core OESDK runtime sources.
$sdkSources = @(
    Get-ChildItem -LiteralPath $sdkSourceRoot -Filter '*.c' -File -Recurse |
        Where-Object {
            -not $_.FullName.StartsWith(
                $libcSourceRoot.TrimEnd('\') + '\',
                [StringComparison]::OrdinalIgnoreCase
            )
        }
)

$projectSources = @()
if (Test-Path -LiteralPath $projectSourceRoot -PathType Container) {
    $projectSources = @(
        Get-ChildItem -LiteralPath $projectSourceRoot -Filter '*.c' -File -Recurse
    )
}

$sources = @($sdkSources) + @($projectSources)
if ($sources.Count -eq 0) { throw 'No C source files were found.' }

Write-Host "OESDK kernel sources: $($sdkSources.Count); project sources: $($projectSources.Count); LibC excluded."

$objects = New-Object System.Collections.Generic.List[string]
$index = 0
foreach ($source in $sources) {
    $object = Join-Path $objectRoot ("Source-{0:D4}.o" -f $index++)
    Write-Host "Clang C  $($source.FullName)"
    & $clang @common -std=c17 -c $source.FullName -o $object
    if ($LASTEXITCODE -ne 0) { throw "Clang failed for $($source.FullName)." }
    $objects.Add($object)
}

$architectureSourceRoot = Join-Path $sdkRoot 'Architecture\x86_64'
$assemblySources = @(Get-ChildItem -LiteralPath $architectureSourceRoot -Filter '*.S' -File | Sort-Object Name)
$assemblyObjects = New-Object System.Collections.Generic.List[string]
foreach ($assemblySource in $assemblySources) {
    $assemblyObject = Join-Path $objectRoot ("Architecture-{0}.o" -f $assemblySource.BaseName)
    Write-Host "Clang ASM $($assemblySource.FullName)"
    & $clang @common -c $assemblySource.FullName -o $assemblyObject
    if ($LASTEXITCODE -ne 0) { throw "Clang failed for $($assemblySource.FullName)." }
    $assemblyObjects.Add($assemblyObject)
}

$sdkSourceCount = $sdkSources.Count
$library = Join-Path $outputRoot 'liboesdk.a'
$sdkObjects = @($objects | Select-Object -First $sdkSourceCount)
& $archiver rcs $library @sdkObjects
if ($LASTEXITCODE -ne 0) { throw 'llvm-ar failed while creating liboesdk.a.' }

$projectObjects = @($objects | Select-Object -Skip $sdkSourceCount)
$kernel = Join-Path $outputRoot 'kernel.elf'
Write-Host "LLD       $kernel"
& $linker -T $linkerScript --build-id=none -z max-page-size=0x1000 -o $kernel @assemblyObjects @projectObjects $library
if ($LASTEXITCODE -ne 0) { throw 'LLD failed while linking the kernel.' }

Write-Host "Built $kernel"
Write-Host "Built $library"
