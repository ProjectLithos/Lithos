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
if (-not (Test-Path -LiteralPath $vsixInstaller)) { throw 'VSIXInstaller.exe could not be found.' }

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

$documents = [Environment]::GetFolderPath('MyDocuments')
if ([string]::IsNullOrWhiteSpace($documents)) { throw 'The Windows Documents folder could not be located.' }
$projectTemplatesRoot = Join-Path $documents 'Visual Studio 2022\Templates\ProjectTemplates'
$ownedTemplateRoot = Join-Path $projectTemplatesRoot 'OESDK'
if (Test-Path -LiteralPath $ownedTemplateRoot) {
    Remove-Item -LiteralPath $ownedTemplateRoot -Recurse -Force
}
if (Test-Path -LiteralPath $projectTemplatesRoot) {
    foreach ($oldTemplate in Get-ChildItem -LiteralPath $projectTemplatesRoot -Filter '*OESDK*.zip' -File -Recurse -ErrorAction SilentlyContinue) {
        Remove-Item -LiteralPath $oldTemplate.FullName -Force
    }
}

Write-Host '[ .. ] Installing exactly two OESDK 0.0.8 native Clang C templates.'
[void][IO.Directory]::CreateDirectory($ownedTemplateRoot)
foreach ($templateName in @('OESDKKernel.zip', 'OESDKDesktop.zip')) {
    $sourceTemplate = Join-Path $sdkRoot (Join-Path 'VisualStudio\ProjectTemplates' $templateName)
    if (-not (Test-Path -LiteralPath $sourceTemplate)) { throw "The installed OESDK template could not be found: $sourceTemplate" }
    Copy-Item -LiteralPath $sourceTemplate -Destination (Join-Path $ownedTemplateRoot $templateName) -Force
}

if (Test-Path -LiteralPath $devenv) {
    Write-Host '[ .. ] Rebuilding the Visual Studio template cache.'
    $refresh = Start-Process -FilePath $devenv -ArgumentList @('/installvstemplates') -Wait -PassThru
    if ($refresh.ExitCode -ne 0) { throw "Visual Studio template refresh failed with code $($refresh.ExitCode)." }
}

Write-Host '[ OK ] Template repair finished. Visual Studio should now show exactly two OESDK 0.0.8 templates.'
