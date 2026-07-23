[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Kernel,
    [int]$MemoryMiB = 256
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. (Join-Path $PSScriptRoot 'Oesdk-DebugTools.ps1')

$qemu = Find-OesdkQemu
$kernelPath = Resolve-OesdkKernel -Kernel $Kernel

$arguments = @(
    '-machine', 'q35',
    '-m', "${MemoryMiB}M",
    '-kernel', $kernelPath,
    '-serial', 'stdio',
    '-no-reboot',
    '-no-shutdown'
)

Write-Host "[ OK ] Running $kernelPath in QEMU."
& $qemu @arguments
exit $LASTEXITCODE
