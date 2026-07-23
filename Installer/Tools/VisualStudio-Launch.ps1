[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$ProjectRoot,
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if (-not ('Oesdk.NativeDebuggerProbe' -as [type])) {
    Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
namespace Oesdk
{
    internal static class NativeDebuggerProbe
    {
        [DllImport("kernel32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool IsDebuggerPresent();
    }
}
'@
}

$project = [IO.Path]::GetFullPath($ProjectRoot.Trim())
$kernel = Join-Path $project ("Build\{0}\kernel.elf" -f $Configuration)
$sdkRoot = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'Machine')
if ([string]::IsNullOrWhiteSpace($sdkRoot)) { $sdkRoot = $env:OESDK_ROOT }
if ([string]::IsNullOrWhiteSpace($sdkRoot)) { $sdkRoot = 'C:\OESDK' }

$underVisualStudioDebugger = [Oesdk.NativeDebuggerProbe]::IsDebuggerPresent()
if ($underVisualStudioDebugger) {
    $tool = Join-Path $sdkRoot 'Tools\Debug-Kernel.ps1'
    Write-Host '[ OK ] F5 detected: starting QEMU paused with GDB.'
    & powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File $tool -Kernel $kernel -BreakAt kmain
} else {
    $tool = Join-Path $sdkRoot 'Tools\Run-Kernel.ps1'
    Write-Host '[ OK ] Ctrl+F5 detected: starting QEMU without GDB.'
    & powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File $tool -Kernel $kernel
}

exit $LASTEXITCODE
