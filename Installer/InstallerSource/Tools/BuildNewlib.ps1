param(
    [string]$Distribution = "",
    [string]$Target = "x86_64-elf"
)
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$LinuxRoot = (wsl.exe $(if ($Distribution) { @("-d", $Distribution) }) wslpath -a $Root).Trim()
if (-not $LinuxRoot) { throw "Unable to translate the OESDK path for WSL." }

$Args = @()
if ($Distribution) { $Args += @("-d", $Distribution) }
$Args += @("env", "OESDK_TARGET=$Target", "bash", "$LinuxRoot/Tools/BuildNewlib.sh")
& wsl.exe @Args
if ($LASTEXITCODE -ne 0) { throw "The Newlib build failed with exit code $LASTEXITCODE." }
Write-Host "[ OK ] Newlib build completed."
