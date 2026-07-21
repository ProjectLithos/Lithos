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

function Find-OesdkGdb {
    $configured = [Environment]::GetEnvironmentVariable('OESDK_GDB', 'User')
    if ([string]::IsNullOrWhiteSpace($configured)) {
        $configured = [Environment]::GetEnvironmentVariable('OESDK_GDB', 'Machine')
    }

    $sdkRoot = Get-OesdkRoot
    $candidates = @(
        $configured,
        (Join-Path $sdkRoot 'Tools\x86_64-elf-gdb.exe'),
        (Join-Path $sdkRoot 'Tools\bin\x86_64-elf-gdb.exe')
    )

    foreach ($candidate in $candidates) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and
            (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return [IO.Path]::GetFullPath($candidate)
        }
    }

    foreach ($name in @('x86_64-elf-gdb.exe', 'gdb.exe')) {
        $command = Get-Command $name -ErrorAction SilentlyContinue
        if ($command) {
            return $command.Source
        }
    }

    throw @'
A GDB debugger was not found.

Install an x86_64-elf GDB build and either:
  * add it to PATH, or
  * set OESDK_GDB to its full path.

PowerShell launches the tools; GDB is the kernel debugger.
'@
}

function Resolve-OesdkKernel {
    param([Parameter(Mandatory = $true)][string]$Kernel)

    $path = [IO.Path]::GetFullPath($Kernel)
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Kernel ELF was not found: $path"
    }
    return $path
}
