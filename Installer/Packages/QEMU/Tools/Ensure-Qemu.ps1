Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$knownPath = Join-Path $env:ProgramFiles 'qemu\qemu-system-x86_64.exe'
$command = Get-Command qemu-system-x86_64.exe -ErrorAction SilentlyContinue
if ($command) { $knownPath = $command.Source }

if (-not (Test-Path $knownPath)) {
    $winget = Get-Command winget.exe -ErrorAction SilentlyContinue
    if (-not $winget) {
        throw 'QEMU is not installed and Windows Package Manager (winget) is unavailable.'
    }
    Write-Host 'Downloading and installing QEMU through Windows Package Manager...'
    & $winget.Source install --id SoftwareFreedomConservancy.QEMU --exact --silent --accept-package-agreements --accept-source-agreements --disable-interactivity
    if ($LASTEXITCODE -ne 0) { throw "winget could not install QEMU (exit code $LASTEXITCODE)." }
}

if (-not (Test-Path $knownPath)) {
    $knownPath = Join-Path $env:ProgramFiles 'qemu\qemu-system-x86_64.exe'
}
if (-not (Test-Path $knownPath)) { throw 'QEMU installation completed but qemu-system-x86_64.exe could not be located.' }

[Environment]::SetEnvironmentVariable('OESDK_QEMU', $knownPath, 'Machine')
$env:OESDK_QEMU = $knownPath
Write-Host "QEMU ready: $knownPath"
