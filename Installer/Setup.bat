@echo off
setlocal EnableExtensions
set "Script=%~dp0Tools\Install-VisualStudio.ps1"
if not exist "%Script%" (
    echo [FAIL] Visual Studio installation script was not found: %Script% 1>&2
    exit /b 1
)
powershell.exe -NoLogo -NoProfile -ExecutionPolicy RemoteSigned -File "%Script%" -OesdkRoot "%~dp0."
exit /b %errorlevel%
