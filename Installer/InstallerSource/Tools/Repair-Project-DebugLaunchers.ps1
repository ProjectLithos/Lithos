
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ProjectDirectory
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$projectRoot = [IO.Path]::GetFullPath($ProjectDirectory)
if (-not (Test-Path -LiteralPath $projectRoot -PathType Container)) {
    throw "Project directory was not found: $projectRoot"
}

$sdkRoot = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'User')
if ([string]::IsNullOrWhiteSpace($sdkRoot)) {
    $sdkRoot = 'C:\OESDK'
}

$templateRoot = Join-Path $sdkRoot 'VisualStudio\Templates\Kernel'
foreach ($name in @('Run-Kernel.cmd', 'Debug-Kernel.cmd')) {
    $source = Join-Path $templateRoot $name
    $destination = Join-Path $projectRoot $name

    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Template launcher was not found: $source"
    }

    Copy-Item -LiteralPath $source -Destination $destination -Force
    Write-Host "[ OK ] Restored $destination"
}
