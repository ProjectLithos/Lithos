@echo off
setlocal EnableExtensions DisableDelayedExpansion

if not defined OESDK_ROOT set "OESDK_ROOT=C:\OESDK"

set "Kernel=%~dp0Build\Debug\kernel.elf"
set "Debugger=%OESDK_ROOT%\Tools\Debug-Kernel.ps1"
set "LauncherLog=%~dp0Build\Debug\oesdk-launcher.log"

if not exist "%~dp0Build\Debug" mkdir "%~dp0Build\Debug" >nul 2>nul

>"%LauncherLog%" echo [ OK ] Debug-Kernel.cmd started.
>>"%LauncherLog%" echo [ OK ] Project directory: %~dp0
>>"%LauncherLog%" echo [ OK ] Kernel: %Kernel%
>>"%LauncherLog%" echo [ OK ] Debugger script: %Debugger%

if not exist "%Kernel%" (
    echo [FAIL] Kernel ELF was not found: %Kernel%
    >>"%LauncherLog%" echo [FAIL] Kernel ELF was not found.
    exit /b 1
)

if not exist "%Debugger%" (
    echo [FAIL] OESDK debugger launcher was not found: %Debugger%
    >>"%LauncherLog%" echo [FAIL] OESDK debugger launcher was not found.
    exit /b 1
)

echo [ OK ] Launching QEMU guest debugger.
>>"%LauncherLog%" echo [ OK ] Starting PowerShell debugger launcher.

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass ^
    -File "%Debugger%" ^
    -Kernel "%Kernel%" ^
    -BreakAt kmain

set "DebugExit=%errorlevel%"
if not defined DebugExit set "DebugExit=1"

>>"%LauncherLog%" echo [ OK ] Debugger exit code: %DebugExit%.

if not "%DebugExit%"=="0" (
    echo [FAIL] Kernel debugger exited with code %DebugExit%.
    echo [FAIL] Launcher log: %LauncherLog%
    echo [FAIL] Debug log: %~dp0Build\Debug\oesdk-debug.log
)

exit /b %DebugExit%
