[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$ProjectName,
    [ValidateSet('Kernel', 'Desktop')][string]$ProjectType = 'Kernel',
    [string]$Destination = (Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'OESDK Projects')
)
# Generated project metadata invokes VisualStudio-Launch.ps1 for F5 debugging.
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ($ProjectName -notmatch '^[A-Za-z_][A-Za-z0-9_.-]*$') {
    throw 'ProjectName must begin with a letter or underscore and contain only letters, numbers, dots, underscores, or hyphens.'
}
$sdkRoot = Split-Path -Parent $PSScriptRoot
$templateRoot = Join-Path $sdkRoot ("VisualStudio\Templates\{0}" -f $ProjectType)
if (-not (Test-Path -LiteralPath $templateRoot -PathType Container)) {
    throw "OESDK template not found: $templateRoot"
}
$projectRoot = Join-Path ([IO.Path]::GetFullPath($Destination)) $ProjectName
if ((Test-Path -LiteralPath $projectRoot) -and @(Get-ChildItem -LiteralPath $projectRoot -Force).Count -ne 0) {
    throw "The project folder already exists and is not empty: $projectRoot"
}
[void][IO.Directory]::CreateDirectory((Join-Path $projectRoot 'Source'))
$guid = [Guid]::NewGuid().ToString('B').ToUpperInvariant()
Get-ChildItem -LiteralPath $templateRoot -File | ForEach-Object {
    if ($_.Extension -eq '.vstemplate') { return }
    $targetName = $_.Name
    if ($targetName -eq 'OESDKKernel.vcxproj' -or $targetName -eq 'OESDKDesktop.vcxproj') {
        $targetName = "$ProjectName.vcxproj"
    }
    $targetRoot = if ($_.Extension -in @('.c', '.h')) { Join-Path $projectRoot 'Source' } else { $projectRoot }
    $text = Get-Content -LiteralPath $_.FullName -Raw
    $text = $text.Replace('$safeprojectname$', $ProjectName).Replace('$projectname$', $ProjectName).Replace('$guid1$', $guid)
    Set-Content -LiteralPath (Join-Path $targetRoot $targetName) -Value $text -Encoding UTF8
}
Write-Host "[ OK ] Created OESDK $ProjectType project: $projectRoot"
