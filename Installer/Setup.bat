@echo off
setlocal EnableExtensions DisableDelayedExpansion

set "OesdkRoot=%~dp0."
set "Script=%~dp0Tools\Install-VisualStudio.ps1"

if not exist "%Script%" (
    echo [FAIL] Visual Studio installation script was not found: %Script% 1>&2
    exit /b 1
)

fltmc.exe >nul 2>nul
if not errorlevel 1 goto RunElevatedSetup

echo [ OK ] Requesting administrator permission for Visual Studio setup.
powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -Command ^
    "$arguments=@('-NoLogo','-NoProfile','-ExecutionPolicy','RemoteSigned','-File','%Script%','-OesdkRoot','%OesdkRoot%');" ^
    "$process=Start-Process -FilePath 'powershell.exe' -ArgumentList $arguments -Verb RunAs -Wait -PassThru;" ^
    "if($null -eq $process){exit 1}; exit [int]$process.ExitCode"

set "ElevatedExit=%errorlevel%"
if not defined ElevatedExit set "ElevatedExit=1"
for /f "delims=0123456789" %%C in ("%ElevatedExit%") do set "ElevatedExit=1"

if "%ElevatedExit%"=="0" goto ElevatedSetupSucceeded

echo [FAIL] Elevated Visual Studio setup exited with code %ElevatedExit%. 1>&2
exit /b %ElevatedExit%

:ElevatedSetupSucceeded
echo [ OK ] Elevated Visual Studio setup completed.
exit /b 0

:RunElevatedSetup
echo [ OK ] Visual Studio setup is running with administrator permission.
powershell.exe -NoLogo -NoProfile -ExecutionPolicy RemoteSigned -File "%Script%" -OesdkRoot "%OesdkRoot%"
set "DirectExit=%errorlevel%"
if not defined DirectExit set "DirectExit=1"
exit /b %DirectExit%
