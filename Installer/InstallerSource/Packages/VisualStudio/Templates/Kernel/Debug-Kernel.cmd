@echo off
setlocal EnableExtensions

if not defined OESDK_ROOT set "OESDK_ROOT=C:\OESDK"
set "Kernel=%~dp0Build\Debug\kernel.elf"
set "Debugger=%OESDK_ROOT%\Tools\Debug-Kernel.ps1"

if not exist "%Kernel%" (
    echo [FAIL] Kernel ELF was not found: %Kernel% 1>&2
    exit /b 1
)
if not exist "%Debugger%" (
    echo [FAIL] OESDK GDB launcher was not found: %Debugger% 1>&2
    exit /b 1
)

echo [ OK ] Visual Studio is launching the OESDK QEMU/GDB kernel debugger.
echo [ OK ] Guest kernel: %Kernel%
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%Debugger%" -Kernel "%Kernel%" -BreakAt kmain
set "DebugExit=%errorlevel%"
if not "%DebugExit%"=="0" (
    echo [FAIL] Kernel debugger exited with code %DebugExit%.
    echo [FAIL] See "%~dp0Build\Debug\oesdk-debug.log".
)
exit /b %DebugExit%
