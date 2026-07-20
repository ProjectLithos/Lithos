param(
    [string]$Destination = "C:\OESDK",
    [switch]$BuildLibC,
    [string]$WslDistribution = ""
)
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
New-Item -ItemType Directory -Force -Path $Destination | Out-Null
Copy-Item -Recurse -Force "$Root\Packages\LibC" "$Destination\LibC"
Copy-Item -Recurse -Force "$Root\Tools" "$Destination\Tools"
Copy-Item -Force "$Root\VERSION" "$Destination\VERSION"
Write-Host "[ OK ] Installed OESDK 0.0.13 libc integration to $Destination."

if ($BuildLibC) {
    & "$Destination\Tools\BuildNewlib.ps1" -Distribution $WslDistribution
    if ($LASTEXITCODE -ne 0) { throw "Newlib build failed." }
}
