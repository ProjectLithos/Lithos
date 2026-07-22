param(
    [string]$OesdkRoot = 'C:\OESDK'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'


function Test-IsAdministrator {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = [Security.Principal.WindowsPrincipal]::new($identity)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

Add-Type -AssemblyName System.Windows.Forms

function Show-Result([string]$Message, [string]$Icon = 'Information') {
    [Windows.Forms.MessageBox]::Show(
        $Message,
        'OESDK Visual Studio Setup',
        'OK',
        $Icon
    ) | Out-Null
}

function Get-VisualStudioInstance {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswhere)) { return $null }

    $raw = (& $vswhere -all -products * -format json -utf8 2>$null | Out-String).Trim()
    if ([string]::IsNullOrWhiteSpace($raw)) { return $null }

    return @($raw | ConvertFrom-Json) |
        Where-Object {
            $_.isComplete -ne $false -and
            $_.isLaunchable -ne $false -and
            $_.productId -in @(
                'Microsoft.VisualStudio.Product.Community',
                'Microsoft.VisualStudio.Product.Professional',
                'Microsoft.VisualStudio.Product.Enterprise'
            )
        } |
        Sort-Object { [version]$_.installationVersion } -Descending |
        Select-Object -First 1
}

function Install-VisualStudioComponents {
    param($Instance)

    $installerRoot = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer'
    $setup = Join-Path $installerRoot 'setup.exe'
    if (-not (Test-Path -LiteralPath $setup)) {
        throw 'The Microsoft Visual Studio Installer is not installed.'
    }

    $arguments = @(
        'modify',
        '--installPath', ('"' + [string]$Instance.installationPath + '"'),
        '--passive',
        '--norestart',
        '--includeRecommended',
        '--add', 'Microsoft.VisualStudio.Workload.NativeDesktop',
        '--add', 'Microsoft.VisualStudio.Component.VC.Llvm.Clang',
        '--add', 'Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Llvm.Clang'
    )

    $process = Start-Process -FilePath $setup -ArgumentList $arguments -Wait -PassThru
    if ($process.ExitCode -notin @(0, 3010)) {
        throw "Microsoft Visual Studio Installer exited with code $($process.ExitCode)."
    }
}


function Remove-LegacyOrynTemplates {
    $documents = [Environment]::GetFolderPath('MyDocuments')
    if ([string]::IsNullOrWhiteSpace($documents)) {
        return
    }

    $templateRoots = @(
        (Join-Path $documents 'Visual Studio 2022\Templates\ProjectTemplates'),
        (Join-Path $documents 'Visual Studio 2022\Templates\ItemTemplates')
    )

    foreach ($templateRoot in $templateRoots) {
        if (-not (Test-Path -LiteralPath $templateRoot -PathType Container)) {
            continue
        }

        Get-ChildItem -LiteralPath $templateRoot -Force -Recurse -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match '(?i)oryn' } |
            Sort-Object { $_.FullName.Length } -Descending |
            ForEach-Object {
                Remove-Item -LiteralPath $_.FullName -Recurse -Force -ErrorAction SilentlyContinue
            }
    }
}

function Install-LocalTemplates {
    param([string]$Root)

    $templateSource = Join-Path $Root 'Packages\VisualStudio\Templates'
    if (-not (Test-Path -LiteralPath $templateSource)) {
        $templateSource = Join-Path $Root 'VisualStudio\ProjectTemplates'
    }
    if (-not (Test-Path -LiteralPath $templateSource)) {
        throw 'The local OESDK Visual Studio templates were not found.'
    }

    $documents = [Environment]::GetFolderPath('MyDocuments')
    if ([string]::IsNullOrWhiteSpace($documents)) {
        throw 'The Documents directory could not be located.'
    }

    $destination = Join-Path $documents 'Visual Studio 2022\Templates\ProjectTemplates\OESDK'
    if (Test-Path -LiteralPath $destination) {
        Remove-Item -LiteralPath $destination -Recurse -Force
    }
    New-Item -ItemType Directory -Force -Path $destination | Out-Null

    foreach ($templateDirectory in Get-ChildItem -LiteralPath $templateSource -Directory) {
        $zipPath = Join-Path $destination ($templateDirectory.Name + '.zip')
        Compress-Archive -Path (Join-Path $templateDirectory.FullName '*') -DestinationPath $zipPath -Force
    }
}

try {
    if (-not (Test-IsAdministrator)) {
        throw 'Visual Studio setup must be run with administrator permission.'
    }
    $cleanRoot = ([string]$OesdkRoot).Trim().Trim('"')
    if ([string]::IsNullOrWhiteSpace($cleanRoot)) {
        throw 'The OESDK installation path was empty.'
    }
    $root = [IO.Path]::GetFullPath($cleanRoot)
    [Environment]::SetEnvironmentVariable('OESDK_ROOT', $root.TrimEnd('\'), 'User')
    $env:OESDK_ROOT = $root.TrimEnd('\')
    if (-not (Test-Path -LiteralPath $root -PathType Container)) {
        throw "OESDK is not installed at $root."
    }

    $instance = Get-VisualStudioInstance
    if (-not $instance) {
        $choice = [Windows.Forms.MessageBox]::Show(
            'Visual Studio 2022 Community, Professional, or Enterprise was not detected. Open the official Microsoft Visual Studio download page?',
            'OESDK Visual Studio Setup',
            'YesNo',
            'Question'
        )
        if ($choice -eq 'Yes') {
            Start-Process 'https://visualstudio.microsoft.com/downloads/'
        }
        throw 'Install Visual Studio 2022, then run C:\OESDK\Setup.bat again.'
    }

    Install-VisualStudioComponents -Instance $instance
    $gdbInstaller = Join-Path $root 'Tools\Ensure-Gdb.ps1'
    if (Test-Path -LiteralPath $gdbInstaller -PathType Leaf) {
        & powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File $gdbInstaller -Quiet
        if ($LASTEXITCODE -ne 0) {
            throw "OESDK GDB installation failed with exit code $LASTEXITCODE."
        }
    }

    Remove-LegacyOrynTemplates

    $warnings = [System.Collections.Generic.List[string]]::new()

    try {
        Install-LocalTemplates -Root $root
    } catch {
        $warnings.Add("Visual Studio components were installed, but the OESDK templates could not be copied: $($_.Exception.Message)")
    }

    try {
        $defaultsPath = Join-Path $root 'OESDKProjectDefaults.json'
        if (-not (Test-Path -LiteralPath $defaultsPath)) {
            [ordered]@{
                AuthorName = ''
                AuthorEmail = ''
                License = 'MIT'
                SdkVersion = '0.0.16'
            } | ConvertTo-Json | Set-Content -LiteralPath $defaultsPath -Encoding UTF8
        }
    } catch {
        $warnings.Add("Visual Studio components were installed, but project defaults could not be written: $($_.Exception.Message)")
    }

    if ($warnings.Count -gt 0) {
        $message = "OESDK Visual Studio components were installed successfully.`r`n`r`n" + ($warnings -join "`r`n")
        Show-Result $message 'Warning'
    } else {
        Show-Result 'OESDK Visual Studio components and project templates were installed successfully. Close and reopen Visual Studio so it reads OESDK_ROOT and the refreshed templates.'
    }
    exit 0
} catch {
    $log = Join-Path $env:TEMP 'OESDK-VisualStudio-Setup.log'
    [IO.File]::WriteAllText($log, ($_ | Out-String))
    Show-Result "$($_.Exception.Message)`r`n`r`nDiagnostic log: $log" 'Error'
    exit 1
}
