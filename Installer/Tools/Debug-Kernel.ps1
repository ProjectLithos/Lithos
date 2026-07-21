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

$projectBuild = Split-Path -Parent ([IO.Path]::GetFullPath($Kernel))
$debugLog = Join-Path $projectBuild 'oesdk-debug.log'
$sessionFile = Join-Path $projectBuild 'Start-Gdb-Session.cmd'

try {
    $qemu = Find-OesdkQemu
    $debugger = Find-OesdkDebugger
    $kernelPath = Resolve-OesdkKernel -Kernel $Kernel
    $serialLog = Join-Path $projectBuild 'serial-debug.log'
    $commandFile = Join-Path $projectBuild 'oesdk-gdb-init.cmd'

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
        'echo \n[ OESDK ] Connected to QEMU.\n',
        'echo [ OESDK ] Type help for GDB commands.\n',
        'echo [ OESDK ] Common commands: continue, nexti, stepi, info registers, backtrace.\n',
        'continue'
    )

    [IO.File]::WriteAllLines($commandFile, $commands, [Text.Encoding]::ASCII)

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

    "[ OK ] Starting QEMU paused on 127.0.0.1:$Port." | Set-Content -LiteralPath $debugLog -Encoding UTF8
    "[ OK ] Kernel symbols: $kernelPath" | Add-Content -LiteralPath $debugLog -Encoding UTF8
    "[ OK ] Guest debugger: $($debugger.Path)" | Add-Content -LiteralPath $debugLog -Encoding UTF8
    "[ OK ] Serial log: $serialLog" | Add-Content -LiteralPath $debugLog -Encoding UTF8
    "[ OK ] GDB init file: $commandFile" | Add-Content -LiteralPath $debugLog -Encoding UTF8

    $qemuProcess = Start-Process -FilePath $qemu -ArgumentList $qemuArguments -PassThru
    Start-Sleep -Milliseconds 1000

    if ($qemuProcess.HasExited) {
        throw "QEMU exited before GDB connected. Exit code: $($qemuProcess.ExitCode)."
    }

    $session = @"
@echo off
title OESDK Kernel Debugger - GDB
echo [ OK ] OESDK interactive kernel debugging session.
echo [ OK ] Kernel: $kernelPath
echo [ OK ] QEMU GDB server: 127.0.0.1:$Port
echo [ OK ] Serial log: $serialLog
echo.
"$($debugger.Path)" -x "$commandFile"
set "GdbExit=%errorlevel%"
echo.
echo [ OK ] GDB exited with code %GdbExit%.
echo [ OK ] Press any key to close this debugger window.
pause >nul
exit /b %GdbExit%
"@

    [IO.File]::WriteAllText(
        $sessionFile,
        $session.Replace("`n", "`r`n"),
        [Text.Encoding]::ASCII
    )

    Write-Host "[ OK ] Opening the interactive GDB console."
    Write-Host "[ OK ] The GDB window remains open for user commands."

    $gdbConsole = Start-Process `
        -FilePath $env:ComSpec `
        -ArgumentList @('/D', '/C', "`"$sessionFile`"") `
        -WorkingDirectory $projectBuild `
        -PassThru `
        -Wait

    $gdbExit = $gdbConsole.ExitCode
    "[ OK ] Interactive GDB console exited with code $gdbExit." |
        Add-Content -LiteralPath $debugLog -Encoding UTF8

    if (-not $qemuProcess.HasExited) {
        Stop-Process -Id $qemuProcess.Id -Force -ErrorAction SilentlyContinue
    }

    if ($gdbExit -ne 0) {
        throw "GDB exited with code $gdbExit."
    }
} catch {
    ($_ | Out-String) | Set-Content -LiteralPath $debugLog -Encoding UTF8
    Write-Host "[FAIL] Kernel debugging could not start."
    Write-Host "[FAIL] $($_.Exception.Message)"
    Write-Host "[FAIL] Diagnostic log: $debugLog"
    exit 1
}

exit 0
