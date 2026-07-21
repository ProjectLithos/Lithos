[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Kernel,
    [switch]$DebugCpu
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Find-Qemu {
    $configured = [Environment]::GetEnvironmentVariable('OESDK_QEMU', 'Machine')
    foreach ($candidate in @(
        $configured,
        (Join-Path $env:ProgramFiles 'qemu\qemu-system-x86_64.exe')
    )) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and (Test-Path $candidate)) { return $candidate }
    }
    $command = Get-Command qemu-system-x86_64.exe -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }
    return $null
}

$qemu = Find-Qemu
if (-not $qemu) {
    $sdkRoot = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'Machine')
    $ensureScript = Join-Path $sdkRoot 'Tools\Ensure-Qemu.ps1'
    if (Test-Path $ensureScript) {
        & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $ensureScript
        if ($LASTEXITCODE -ne 0) { throw 'QEMU installation failed.' }
        $qemu = Find-Qemu
    }
}
if (-not $qemu) { throw 'QEMU was not found. Repair the OESDK QEMU package.' }

$kernelPath = [IO.Path]::GetFullPath($Kernel)
if (-not (Test-Path $kernelPath)) { throw "Kernel image not found: $kernelPath" }
$arguments = @('-machine', 'q35', '-m', '256M', '-kernel', $kernelPath, '-serial', 'stdio', '-no-reboot', '-no-shutdown')
if ($DebugCpu) { $arguments += @('-S', '-s') }
& $qemu @arguments
exit $LASTEXITCODE
