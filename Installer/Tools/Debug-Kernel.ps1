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

try {
    $qemu = Find-OesdkQemu
    $debugger = Find-OesdkDebugger
    $kernelPath = Resolve-OesdkKernel -Kernel $Kernel
    $serialLog = Join-Path $projectBuild 'serial-debug.log'
    $commandFile = Join-Path $env:TEMP ("OESDK-Debug-{0}.cmd" -f [Guid]::NewGuid().ToString('N'))
    $escapedKernel = $kernelPath.Replace('\', '/').Replace('"', '\"')

    if ($debugger.Kind -eq 'GDB') {
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
        $commands += 'continue'
    } else {
        $commands = @(
            'settings set auto-confirm true',
            ('target create "' + $escapedKernel + '"'),
            ('gdb-remote 127.0.0.1:' + $Port)
        )
        if (-not [string]::IsNullOrWhiteSpace($BreakAt)) {
            $commands += ('breakpoint set --name ' + $BreakAt)
        }
        $commands += 'continue'
    }

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

    $qemuProcess = $null
    try {
        "[ OK ] Starting QEMU paused on 127.0.0.1:$Port." | Tee-Object -FilePath $debugLog
        "[ OK ] Kernel symbols: $kernelPath" | Tee-Object -FilePath $debugLog -Append
        "[ OK ] Guest debugger: $($debugger.Kind) - $($debugger.Path)" | Tee-Object -FilePath $debugLog -Append
        "[ OK ] Serial log: $serialLog" | Tee-Object -FilePath $debugLog -Append

        $qemuProcess = Start-Process -FilePath $qemu -ArgumentList $qemuArguments -PassThru
        Start-Sleep -Milliseconds 1000

        if ($qemuProcess.HasExited) {
            throw "QEMU exited before the debugger connected. Exit code: $($qemuProcess.ExitCode)."
        }

        if ($debugger.Kind -eq 'GDB') {
            & $debugger.Path '-x' $commandFile 2>&1 |
                Tee-Object -FilePath $debugLog -Append
        } else {
            & $debugger.Path '-s' $commandFile 2>&1 |
                Tee-Object -FilePath $debugLog -Append
        }

        $debuggerExit = $LASTEXITCODE
        if ($debuggerExit -ne 0) {
            throw "$($debugger.Kind) exited with code $debuggerExit."
        }
    } finally {
        if ($qemuProcess -and -not $qemuProcess.HasExited) {
            Stop-Process -Id $qemuProcess.Id -Force -ErrorAction SilentlyContinue
        }
        Remove-Item -LiteralPath $commandFile -Force -ErrorAction SilentlyContinue
    }
} catch {
    ($_ | Out-String) | Set-Content -LiteralPath $debugLog -Encoding UTF8
    Write-Host "[FAIL] Kernel debugging could not start."
    Write-Host "[FAIL] $($_.Exception.Message)"
    Write-Host "[FAIL] Diagnostic log: $debugLog"
    exit 1
}

exit 0
