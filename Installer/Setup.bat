@echo off
setlocal EnableExtensions

set "OesdkRoot=%~dp0."
set "Script=%~dp0Tools\Install-VisualStudio.ps1"

if not exist "%Script%" (
    echo [FAIL] Visual Studio installation script was not found: %Script% 1>&2
    exit /b 1
)

fltmc.exe >nul 2>nul
if errorlevel 1 (
    echo [ OK ] Requesting administrator permission for Visual Studio setup.
    powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -Command ^
        "$arguments=@('-NoLogo','-NoProfile','-ExecutionPolicy','RemoteSigned','-File','%Script%','-OesdkRoot','%OesdkRoot%');" ^
        "$process=Start-Process -FilePath 'powershell.exe' -ArgumentList $arguments -Verb RunAs -Wait -PassThru;" ^
        "exit $process.ExitCode"
    set "ElevatedExit=%errorlevel%"
    if "%ElevatedExit%"=="0" (
        echo [ OK ] Elevated Visual Studio setup completed.
    ) else (
        echo [FAIL] Elevated Visual Studio setup exited with code %ElevatedExit%. 1>&2
    )
    exit /b %ElevatedExit%
)

echo [ OK ] Visual Studio setup is running with administrator permission.
powershell.exe -NoLogo -NoProfile -ExecutionPolicy RemoteSigned -File "%Script%" -OesdkRoot "%OesdkRoot%"
exit /b %errorlevel%
