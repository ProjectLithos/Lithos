@echo off
setlocal
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%OESDK_ROOT%\Tools\Debug-Kernel.ps1" -Kernel "%~dp0Build\Debug\kernel.elf" -BreakAt kmain
exit /b %errorlevel%
