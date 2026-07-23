[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$ProjectRoot,
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$project = $null
$log = $null
try {
    $project = [IO.Path]::GetFullPath($ProjectRoot.Trim().TrimEnd('\\'))
    $buildDirectory = Join-Path $project ("Build\{0}" -f $Configuration)
    [IO.Directory]::CreateDirectory($buildDirectory) | Out-Null
    $log = Join-Path $buildDirectory 'oesdk-visualstudio-launch.log'

    function Write-LauncherLog([string]$Message) {
        $line = "[{0:yyyy-MM-dd HH:mm:ss.fff}] {1}" -f [DateTime]::Now, $Message
        Write-Host $line
        Add-Content -LiteralPath $log -Value $line -Encoding UTF8
    }

    Set-Content -LiteralPath $log -Value '' -Encoding UTF8
    Write-LauncherLog "OESDK Visual Studio launcher started."
    Write-LauncherLog "Project: $project"
    Write-LauncherLog "Configuration: $Configuration"

    $kernel = Join-Path $buildDirectory 'kernel.elf'
    if (-not (Test-Path -LiteralPath $kernel -PathType Leaf)) {
        throw "Kernel ELF was not found: $kernel"
    }

    $sdkRoot = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'Machine')
    if ([string]::IsNullOrWhiteSpace($sdkRoot)) { $sdkRoot = $env:OESDK_ROOT }
    if ([string]::IsNullOrWhiteSpace($sdkRoot)) { $sdkRoot = 'C:\OESDK' }
    $sdkRoot = [IO.Path]::GetFullPath($sdkRoot.Trim().TrimEnd('\\'))
    Write-LauncherLog "SDK root: $sdkRoot"

    if (-not ('Oesdk.NativeDebuggerProbe' -as [type])) {
        Add-Type -TypeDefinition @'
using System.Runtime.InteropServices;
namespace Oesdk {
    internal static class NativeDebuggerProbe {
        [DllImport("kernel32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool IsDebuggerPresent();
    }
}
'@
    }

    $underDebugger = [Oesdk.NativeDebuggerProbe]::IsDebuggerPresent()
    if ($underDebugger) {
        $tool = Join-Path $sdkRoot 'Tools\Debug-Kernel.ps1'
        $mode = 'F5/debug'
        $arguments = @('-NoLogo','-NoProfile','-ExecutionPolicy','Bypass','-File',$tool,'-Kernel',$kernel,'-BreakAt','kmain')
    } else {
        $tool = Join-Path $sdkRoot 'Tools\Run-Kernel.ps1'
        $mode = 'Ctrl+F5/run'
        $arguments = @('-NoLogo','-NoProfile','-ExecutionPolicy','Bypass','-File',$tool,'-Kernel',$kernel)
    }

    if (-not (Test-Path -LiteralPath $tool -PathType Leaf)) {
        throw "OESDK launcher tool was not found: $tool"
    }

    Write-LauncherLog "Mode: $mode"
    Write-LauncherLog "Tool: $tool"
    & powershell.exe @arguments
    $exitCode = $LASTEXITCODE
    Write-LauncherLog "Child launcher exit code: $exitCode"
    if ($exitCode -ne 0) { throw "The OESDK $mode launcher failed with exit code $exitCode." }
    exit 0
}
catch {
    $message = $_.Exception.Message
    $details = ($_ | Out-String).Trim()
    if ([string]::IsNullOrWhiteSpace($log)) {
        $log = Join-Path $env:TEMP 'oesdk-visualstudio-launch.log'
    }
    try {
        Add-Content -LiteralPath $log -Value ("[FAIL] {0}`r`n{1}" -f $message, $details) -Encoding UTF8
    } catch { }
    Write-Host "[FAIL] $message" -ForegroundColor Red
    Write-Host "[FAIL] Full details: $log" -ForegroundColor Red
    Write-Host ''
    Write-Host 'Press Enter to close this window.'
    try { [void][Console]::ReadLine() } catch { Start-Sleep -Seconds 15 }
    exit 1
}
