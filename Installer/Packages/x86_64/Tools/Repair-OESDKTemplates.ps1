[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if (Get-Process -Name devenv -ErrorAction SilentlyContinue) {
    throw 'Close every Visual Studio window before running this repair.'
}

$installerRoot = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer'
$vswhere = Join-Path $installerRoot 'vswhere.exe'
if (-not (Test-Path -LiteralPath $vswhere)) { throw 'Visual Studio Installer could not be found.' }
$vsPath = (& $vswhere -latest -products * -property installationPath | Out-String).Trim()
if ([string]::IsNullOrWhiteSpace($vsPath)) { throw 'A Visual Studio installation could not be found.' }

$vsixInstaller = Join-Path $vsPath 'Common7\IDE\VSIXInstaller.exe'
$devenv = Join-Path $vsPath 'Common7\IDE\devenv.com'
$sdkRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$vsixPath = Join-Path $sdkRoot 'VisualStudio\OESDK.VisualStudio.vsix'
if (-not (Test-Path -LiteralPath $vsixInstaller)) { throw 'VSIXInstaller.exe could not be found.' }
if (-not (Test-Path -LiteralPath $vsixPath)) { throw "The installed OESDK VSIX could not be found: $vsixPath" }

$extensionIds = New-Object 'System.Collections.Generic.HashSet[string]' ([StringComparer]::OrdinalIgnoreCase)
[void]$extensionIds.Add('ProjectLithos.OESDK.VisualStudio')
$extensionRoot = Join-Path $env:LOCALAPPDATA 'Microsoft\VisualStudio'
if (Test-Path -LiteralPath $extensionRoot) {
    foreach ($manifestPath in Get-ChildItem -LiteralPath $extensionRoot -Filter 'extension.vsixmanifest' -File -Recurse -ErrorAction SilentlyContinue) {
        try {
            $text = [IO.File]::ReadAllText($manifestPath.FullName)
            if ($text -match '(?i)OESDK' -and $text -match '(?i)<Identity\s+[^>]*Id="([^"]+)"') {
                [void]$extensionIds.Add($Matches[1])
            }
        } catch { }
    }
}

foreach ($extensionId in $extensionIds) {
    Write-Host "[ .. ] Removing obsolete OESDK extension identity: $extensionId"
    $uninstall = Start-Process -FilePath $vsixInstaller -ArgumentList @('/quiet', "/uninstall:$extensionId") -Wait -PassThru
}

Write-Host '[ .. ] Installing the OESDK 0.0.7 native Clang C templates.'
$install = Start-Process -FilePath $vsixInstaller -ArgumentList @('/quiet', ('"' + $vsixPath + '"')) -Wait -PassThru
if ($install.ExitCode -ne 0) { throw "VSIX installation failed with code $($install.ExitCode)." }

if (Test-Path -LiteralPath $devenv) {
    Write-Host '[ .. ] Rebuilding the Visual Studio template cache.'
    $refresh = Start-Process -FilePath $devenv -ArgumentList @('/installvstemplates') -Wait -PassThru
    if ($refresh.ExitCode -ne 0) { throw "Visual Studio template refresh failed with code $($refresh.ExitCode)." }
}

Write-Host '[ OK ] Template repair finished. Visual Studio should now show exactly two OESDK 0.0.7 templates.'
