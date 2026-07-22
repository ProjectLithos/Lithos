@echo off
setlocal
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0Tools\Install-OESDK-0.0.16.ps1" %*
exit /b %errorlevel%
