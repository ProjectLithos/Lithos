@echo off
setlocal EnableExtensions

if "%~1"=="" goto usage
set "OESDK_PROJECT_NAME=%~1"
set "OESDK_PROJECT_TYPE=%~2"
if not defined OESDK_PROJECT_TYPE set "OESDK_PROJECT_TYPE=Kernel"
set "OESDK_PROJECT_DEST=%~3"
if not defined OESDK_PROJECT_DEST set "OESDK_PROJECT_DEST=%USERPROFILE%\source\repos"

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0New-OESDKProject.ps1" -ProjectName "%OESDK_PROJECT_NAME%" -ProjectType "%OESDK_PROJECT_TYPE%" -Destination "%OESDK_PROJECT_DEST%"
exit /b %ERRORLEVEL%

:usage
echo Usage: New-OESDKProject.bat PROJECT_NAME [Kernel^|Desktop] [DESTINATION]
echo Example: New-OESDKProject.bat Koryn Kernel "%USERPROFILE%\source\repos"
exit /b 2
