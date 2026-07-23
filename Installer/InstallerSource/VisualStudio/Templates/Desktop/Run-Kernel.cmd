@echo off
setlocal
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%OESDK_ROOT%\Tools\Run-Kernel.ps1" -Kernel "%~dp0Build\Debug\kernel.elf"
exit /b %errorlevel%
