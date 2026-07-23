Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-OesdkRoot {
    $root = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'User')
    if ([string]::IsNullOrWhiteSpace($root)) {
        $root = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'Machine')
    }
    if ([string]::IsNullOrWhiteSpace($root)) {
        $root = 'C:\OESDK'
    }
    return [IO.Path]::GetFullPath($root)
}

function Find-OesdkQemu {
    $configured = [Environment]::GetEnvironmentVariable('OESDK_QEMU', 'User')
    if ([string]::IsNullOrWhiteSpace($configured)) {
        $configured = [Environment]::GetEnvironmentVariable('OESDK_QEMU', 'Machine')
    }

    $candidates = @(
        $configured,
        (Join-Path $env:ProgramFiles 'qemu\qemu-system-x86_64.exe'),
        (Join-Path ${env:ProgramFiles(x86)} 'qemu\qemu-system-x86_64.exe')
    )

    foreach ($candidate in $candidates) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and
            (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return [IO.Path]::GetFullPath($candidate)
        }
    }

    $command = Get-Command qemu-system-x86_64.exe -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }

    throw 'QEMU was not found. Run C:\OESDK\Tools\Ensure-Qemu.ps1 first.'
}

function Find-OesdkDebugger {
    $configured = [Environment]::GetEnvironmentVariable('OESDK_GDB', 'User')
    if ([string]::IsNullOrWhiteSpace($configured)) {
        $configured = [Environment]::GetEnvironmentVariable('OESDK_GDB', 'Machine')
    }

    $sdkRoot = Get-OesdkRoot
    $candidates = @(
        $configured,
        'C:\msys64\ucrt64\bin\gdb.exe',
        'C:\msys64\ucrt64\bin\gdb-multiarch.exe',
        (Join-Path $sdkRoot 'Tools\x86_64-elf-gdb.exe'),
        (Join-Path $sdkRoot 'Tools\bin\x86_64-elf-gdb.exe')
    )

    foreach ($candidate in $candidates) {
        if ([string]::IsNullOrWhiteSpace($candidate) -or
            -not (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            continue
        }

        & $candidate --version *> $null
        if ($LASTEXITCODE -eq 0) {
            return [pscustomobject]@{
                Kind = 'GDB'
                Path = [IO.Path]::GetFullPath($candidate)
            }
        }
    }

    foreach ($name in @('gdb.exe', 'x86_64-elf-gdb.exe', 'gdb-multiarch.exe')) {
        $command = Get-Command $name -ErrorAction SilentlyContinue
        if ($command) {
            & $command.Source --version *> $null
            if ($LASTEXITCODE -eq 0) {
                return [pscustomobject]@{
                    Kind = 'GDB'
                    Path = $command.Source
                }
            }
        }
    }

    $installer = Join-Path $sdkRoot 'Tools\Ensure-Gdb.ps1'
    throw @"
A working GNU debugger was not found.

Run:
  powershell.exe -ExecutionPolicy Bypass -File "$installer"

Visual Studio LLDB is not used because the installed lldb.exe can fail with
Windows loader error 0xC0000135 when its dependent DLLs are unavailable.
"@
}

function Resolve-OesdkKernel {
    param([Parameter(Mandatory = $true)][string]$Kernel)

    $path = [IO.Path]::GetFullPath($Kernel)
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Kernel ELF was not found: $path"
    }
    return $path
}
