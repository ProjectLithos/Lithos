[CmdletBinding()]
param(
    [string]$Distribution = '',
    [string]$Target = 'x86_64-elf'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'User')
if ([string]::IsNullOrWhiteSpace($root)) {
    $root = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'Machine')
}
if ([string]::IsNullOrWhiteSpace($root)) {
    $root = 'C:\OESDK'
}

$builder = Join-Path $root 'Tools\BuildNewlib.ps1'
if (-not (Test-Path -LiteralPath $builder -PathType Leaf)) {
    throw "The Newlib build script was not found at $builder."
}

& $builder -Distribution $Distribution -Target $Target
if ($LASTEXITCODE -ne 0) {
    throw "The Newlib build failed with exit code $LASTEXITCODE."
}
