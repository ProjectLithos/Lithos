@echo off
setlocal EnableExtensions

set "OesdkRoot=%~dp0."
set "Script=%~dp0Tools\Install-VisualStudio.ps1"

if not exist "%Script%" (
    echo [FAIL] Visual Studio installation script was not found: %Script% 1>&2
    exit /b 1
)

rem Passive Visual Studio Installer operations must start elevated.
fltmc.exe >nul 2>nul
if errorlevel 1 (
    echo [ OK ] Requesting administrator permission for Visual Studio setup.
    powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -Command ^
        "$p=Start-Process -FilePath $env:ComSpec -ArgumentList @('/D','/S','/C','""%~f0"" --elevated') -Verb RunAs -Wait -PassThru; exit $p.ExitCode"
    exit /b %errorlevel%
)

if /I "%~1"=="--elevated" (
    echo [ OK ] Visual Studio setup is running with administrator permission.
)

powershell.exe -NoLogo -NoProfile -ExecutionPolicy RemoteSigned -File "%Script%" -OesdkRoot "%OesdkRoot%"
exit /b %errorlevel%
