@echo off
setlocal
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0Tools\Install-OESDK-0.0.13.ps1" %*
exit /b %errorlevel%
