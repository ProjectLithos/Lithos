[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Kernel,
    [int]$Port = 1234,
    [string]$BreakAt = 'kmain',
    [int]$MemoryMiB = 256
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. (Join-Path $PSScriptRoot 'Oesdk-DebugTools.ps1')

$qemu = Find-OesdkQemu
$gdb = Find-OesdkGdb
$kernelPath = Resolve-OesdkKernel -Kernel $Kernel
$projectBuild = Split-Path -Parent $kernelPath
$serialLog = Join-Path $projectBuild 'serial-debug.log'
$gdbCommands = Join-Path $env:TEMP ("OESDK-GDB-{0}.cmd" -f [Guid]::NewGuid().ToString('N'))

$escapedKernel = $kernelPath.Replace('\', '/').Replace('"', '\"')
$commands = @(
    'set confirm off',
    'set pagination off',
    'set disassembly-flavor intel',
    ('file "' + $escapedKernel + '"'),
    ('target remote 127.0.0.1:' + $Port)
)

if (-not [string]::IsNullOrWhiteSpace($BreakAt)) {
    $commands += ('break ' + $BreakAt)
}
$commands += @(
    'continue'
)

[IO.File]::WriteAllLines($gdbCommands, $commands, [Text.Encoding]::ASCII)

$qemuArguments = @(
    '-machine', 'q35',
    '-m', "${MemoryMiB}M",
    '-kernel', $kernelPath,
    '-serial', ('file:' + $serialLog),
    '-no-reboot',
    '-no-shutdown',
    '-S',
    '-gdb', ('tcp:127.0.0.1:' + $Port)
)

$qemuProcess = $null
try {
    Write-Host "[ OK ] Starting QEMU paused with GDB server on 127.0.0.1:$Port."
    Write-Host "[ OK ] Kernel symbols: $kernelPath"
    Write-Host "[ OK ] Serial log: $serialLog"
    Write-Host "[ OK ] Debugger: $gdb"

    $qemuProcess = Start-Process -FilePath $qemu -ArgumentList $qemuArguments -PassThru
    Start-Sleep -Milliseconds 800

    if ($qemuProcess.HasExited) {
        throw "QEMU exited before GDB connected. Exit code: $($qemuProcess.ExitCode)."
    }

    & $gdb '-x' $gdbCommands
    $gdbExit = $LASTEXITCODE
    if ($gdbExit -ne 0) {
        throw "GDB exited with code $gdbExit."
    }
} finally {
    if ($qemuProcess -and -not $qemuProcess.HasExited) {
        Stop-Process -Id $qemuProcess.Id -Force -ErrorAction SilentlyContinue
    }
    Remove-Item -LiteralPath $gdbCommands -Force -ErrorAction SilentlyContinue
}
