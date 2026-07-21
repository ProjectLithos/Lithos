[CmdletBinding()]
param(
    [switch]$Quiet
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Write-Status([string]$Message) {
    if (-not $Quiet) {
        Write-Host $Message
    }
}

$gdbCandidates = @(
    'C:\msys64\ucrt64\bin\gdb-multiarch.exe',
    'C:\msys64\ucrt64\bin\gdb.exe'
)

foreach ($candidate in $gdbCandidates) {
    if (Test-Path -LiteralPath $candidate -PathType Leaf) {
        & $candidate --version *> $null
        if ($LASTEXITCODE -eq 0) {
            [Environment]::SetEnvironmentVariable('OESDK_GDB', $candidate, 'User')
            Write-Status "[ OK ] OESDK GDB is available at $candidate"
            exit 0
        }
    }
}

$winget = Get-Command winget.exe -ErrorAction SilentlyContinue
if (-not $winget) {
    throw 'Windows Package Manager (winget) is required to install MSYS2 GDB automatically.'
}

Write-Status '[ OK ] Installing MSYS2 for the OESDK kernel debugger.'
& $winget.Source install `
    --id MSYS2.MSYS2 `
    --exact `
    --silent `
    --accept-package-agreements `
    --accept-source-agreements `
    --disable-interactivity

$wingetExit = $LASTEXITCODE
if ($wingetExit -ne 0 -and $wingetExit -ne -1978335189) {
    throw "MSYS2 installation failed with winget exit code $wingetExit."
}

$bash = 'C:\msys64\usr\bin\bash.exe'
if (-not (Test-Path -LiteralPath $bash -PathType Leaf)) {
    throw "MSYS2 was installed, but bash.exe was not found at $bash."
}

Write-Status '[ OK ] Updating the MSYS2 package database.'
& $bash -lc 'pacman -Sy --noconfirm'
if ($LASTEXITCODE -ne 0) {
    throw 'MSYS2 package database update failed.'
}

Write-Status '[ OK ] Installing the multi-architecture GNU debugger.'
& $bash -lc 'pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gdb-multiarch'
if ($LASTEXITCODE -ne 0) {
    throw 'MSYS2 GDB installation failed.'
}

$gdb = 'C:\msys64\ucrt64\bin\gdb-multiarch.exe'
if (-not (Test-Path -LiteralPath $gdb -PathType Leaf)) {
    throw "GDB installation completed, but $gdb was not found."
}

& $gdb --version *> $null
if ($LASTEXITCODE -ne 0) {
    throw "GDB was found but could not run successfully: $gdb"
}

[Environment]::SetEnvironmentVariable('OESDK_GDB', $gdb, 'User')
$env:OESDK_GDB = $gdb
Write-Status "[ OK ] OESDK GDB installed at $gdb"
exit 0
