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
    $configuredGdb = [Environment]::GetEnvironmentVariable('OESDK_GDB', 'User')
    if ([string]::IsNullOrWhiteSpace($configuredGdb)) {
        $configuredGdb = [Environment]::GetEnvironmentVariable('OESDK_GDB', 'Machine')
    }

    $sdkRoot = Get-OesdkRoot
    $gdbCandidates = @(
        $configuredGdb,
        (Join-Path $sdkRoot 'Tools\x86_64-elf-gdb.exe'),
        (Join-Path $sdkRoot 'Tools\bin\x86_64-elf-gdb.exe')
    )

    foreach ($candidate in $gdbCandidates) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and
            (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return [pscustomobject]@{ Kind = 'GDB'; Path = [IO.Path]::GetFullPath($candidate) }
        }
    }

    foreach ($name in @('x86_64-elf-gdb.exe', 'gdb.exe')) {
        $command = Get-Command $name -ErrorAction SilentlyContinue
        if ($command) {
            return [pscustomobject]@{ Kind = 'GDB'; Path = $command.Source }
        }
    }

    $configuredLldb = [Environment]::GetEnvironmentVariable('OESDK_LLDB', 'User')
    if ([string]::IsNullOrWhiteSpace($configuredLldb)) {
        $configuredLldb = [Environment]::GetEnvironmentVariable('OESDK_LLDB', 'Machine')
    }

    $vsRoots = @(
        (Join-Path $env:ProgramFiles 'Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\lldb.exe'),
        (Join-Path $env:ProgramFiles 'Microsoft Visual Studio\2022\Professional\VC\Tools\Llvm\x64\bin\lldb.exe'),
        (Join-Path $env:ProgramFiles 'Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\x64\bin\lldb.exe')
    )

    $lldbCandidates = @($configuredLldb) + $vsRoots
    foreach ($candidate in $lldbCandidates) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and
            (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return [pscustomobject]@{ Kind = 'LLDB'; Path = [IO.Path]::GetFullPath($candidate) }
        }
    }

    $lldbCommand = Get-Command lldb.exe -ErrorAction SilentlyContinue
    if ($lldbCommand) {
        return [pscustomobject]@{ Kind = 'LLDB'; Path = $lldbCommand.Source }
    }

    throw @'
No guest kernel debugger was found.

Install x86_64-elf-gdb, or install Visual Studio's LLVM/Clang component so
lldb.exe is available. You may also set OESDK_GDB or OESDK_LLDB to the full
debugger path.
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
