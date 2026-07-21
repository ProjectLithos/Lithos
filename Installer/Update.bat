@echo off
setlocal EnableExtensions DisableDelayedExpansion

set "UpdaterVersion=0.0.14.3"
set "InstallRoot=C:\OESDK"
set "Repository=ProjectLithos/Lithos"
set "Branch=main"
set "RequestedPackage=%~1"
set "ExitCode=1"

if defined OESDK_REPOSITORY set "Repository=%OESDK_REPOSITORY%"
if defined OESDK_BRANCH set "Branch=%OESDK_BRANCH%"

echo [ OK ] OESDK updater %UpdaterVersion%.

rem ============================================================================
rem Stage this updater in TEMP before C:\OESDK is replaced.
rem A separate cmd.exe process is used instead of CALL so batch context and
rem parenthesized-block expansion cannot lose the temporary path.
rem ============================================================================
if defined OESDK_UPDATE_STAGED goto CheckForNewUpdater

if not defined TEMP set "TEMP=%SystemRoot%\Temp"
set "StageBatch=%TEMP%\OESDK-Update-Stage-%RANDOM%-%RANDOM%.bat"
copy /B /Y "%~f0" "%StageBatch%" >nul
if errorlevel 1 (
    echo [FAIL] Unable to copy Update.bat to the temporary staging path. 1>&2
    exit /b 1
)
if not exist "%StageBatch%" (
    echo [FAIL] The staged updater does not exist: %StageBatch% 1>&2
    exit /b 1
)

echo [ OK ] Staged updater: %StageBatch%.
set "OESDK_UPDATE_STAGED=1"
set "OESDK_SELF_UPDATE_CHECKED="

if defined RequestedPackage (
    "%ComSpec%" /D /S /C ""%StageBatch%" "%RequestedPackage%""
) else (
    "%ComSpec%" /D /S /C ""%StageBatch%""
)
set "ChildExit=%errorlevel%"
del /Q "%StageBatch%" >nul 2>nul
exit /b %ChildExit%

:CheckForNewUpdater
rem ============================================================================
rem Look only in the current user's Downloads directory:
rem     %USERPROFILE%\Downloads\Update*.bat
rem Compare the UpdaterVersion inside each file. If none is newer, continue
rem immediately to selection and extraction of the latest complete source ZIP.
rem ============================================================================
if defined OESDK_SELF_UPDATE_CHECKED goto NoNewUpdater

set "OESDK_SELF_UPDATE_CHECKED=1"
set "OESDK_CURRENT_UPDATER=%~f0"
set "OESDK_DOWNLOADS=%USERPROFILE%\Downloads"
set "NewUpdaterFile=%TEMP%\OESDK-NewUpdater-%RANDOM%-%RANDOM%.txt"
set "OESDK_NEW_UPDATER_FILE=%NewUpdaterFile%"

powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
  "$ErrorActionPreference='Stop';" ^
  "$pattern='UpdaterVersion=([0-9]+(?:\.[0-9]+){2,3})';" ^
  "$currentText=[IO.File]::ReadAllText($env:OESDK_CURRENT_UPDATER);" ^
  "$currentMatch=[regex]::Match($currentText,$pattern,[Text.RegularExpressions.RegexOptions]::IgnoreCase);" ^
  "if(-not $currentMatch.Success){exit 2};" ^
  "$current=[version]$currentMatch.Groups[1].Value;" ^
  "$best=$null;$bestVersion=$current;" ^
  "Get-ChildItem -LiteralPath $env:OESDK_DOWNLOADS -File -Filter 'Update*.bat' -ErrorAction SilentlyContinue | ForEach-Object {" ^
  " try{" ^
  "  $candidateText=[IO.File]::ReadAllText($_.FullName);" ^
  "  $candidateMatch=[regex]::Match($candidateText,$pattern,[Text.RegularExpressions.RegexOptions]::IgnoreCase);" ^
  "  if($candidateMatch.Success){" ^
  "   $candidateVersion=[version]$candidateMatch.Groups[1].Value;" ^
  "   if($candidateVersion -gt $bestVersion){$best=$_;$bestVersion=$candidateVersion}" ^
  "  }" ^
  " }catch{}" ^
  "};" ^
  "$value=if($best){$best.FullName}else{''};" ^
  "[IO.File]::WriteAllText($env:OESDK_NEW_UPDATER_FILE,$value,[Text.Encoding]::ASCII)"

if errorlevel 2 (
    echo [WARN] The current updater version could not be parsed.
    goto NoNewUpdater
)
if errorlevel 1 (
    echo [WARN] Downloads could not be checked for a newer updater.
    goto NoNewUpdater
)

set "NewUpdater="
if exist "%NewUpdaterFile%" set /P "NewUpdater="<"%NewUpdaterFile%"
del /Q "%NewUpdaterFile%" >nul 2>nul
if not defined NewUpdater goto NoNewUpdater

if not exist "%NewUpdater%" (
    echo [WARN] The selected newer updater no longer exists; continuing with the source ZIP.
    goto NoNewUpdater
)

set "RestartBatch=%TEMP%\OESDK-NewUpdater-Run-%RANDOM%-%RANDOM%.bat"
copy /B /Y "%NewUpdater%" "%RestartBatch%" >nul
if errorlevel 1 (
    echo [WARN] The newer updater could not be staged; continuing with the source ZIP.
    goto NoNewUpdater
)
if not exist "%RestartBatch%" (
    echo [WARN] The staged newer updater was not found; continuing with the source ZIP.
    goto NoNewUpdater
)

copy /B /Y "%NewUpdater%" "%InstallRoot%\Update.bat" >nul
if errorlevel 1 (
    echo [WARN] The newer updater could not replace C:\OESDK\Update.bat; running it from TEMP.
) else (
    echo [ OK ] Installed newer updater: %NewUpdater%.
)

echo [ OK ] Restarting under newer updater: %RestartBatch%.
set "OESDK_UPDATE_STAGED=1"
set "OESDK_SELF_UPDATE_CHECKED=1"
if defined RequestedPackage (
    "%ComSpec%" /D /S /C ""%RestartBatch%" "%RequestedPackage%""
) else (
    "%ComSpec%" /D /S /C ""%RestartBatch%""
)
set "RestartExit=%errorlevel%"
del /Q "%RestartBatch%" >nul 2>nul
exit /b %RestartExit%

:NoNewUpdater
echo [ OK ] No newer Update.bat was found.
echo [ OK ] Continuing to select and extract the latest complete source ZIP.
goto StagedReady

:StagedReady

for %%T in (powershell.exe git.exe robocopy.exe) do (
    where %%T >nul 2>nul
    if errorlevel 1 (
        call :Fail "Required tool not found: %%T"
        goto Cleanup
    )
)

set "WorkRoot=%TEMP%\OESDK-Update-Work-%RANDOM%-%RANDOM%"
set "ExtractRoot=%WorkRoot%\Extracted"
set "Checkout=%WorkRoot%\Checkout"
set "PackageFile=%WorkRoot%\Package.txt"
set "RootFile=%WorkRoot%\Root.txt"
set "VersionFile=%WorkRoot%\Version.txt"
mkdir "%ExtractRoot%" >nul 2>nul
if errorlevel 1 (
    call :Fail "Unable to create updater workspace."
    goto Cleanup
)

rem Select the highest-version complete source ZIP, not simply the newest file timestamp.
set "OESDK_REQUESTED_PACKAGE=%RequestedPackage%"
set "OESDK_DOWNLOADS=%USERPROFILE%\Downloads"
set "OESDK_PACKAGE_FILE=%PackageFile%"

powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
  "$ErrorActionPreference='Stop';Add-Type -AssemblyName System.IO.Compression.FileSystem;" ^
  "$requested=$env:OESDK_REQUESTED_PACKAGE;" ^
  "if(-not [string]::IsNullOrWhiteSpace($requested)){" ^
  " if(-not(Test-Path -LiteralPath $requested -PathType Leaf)){throw 'Requested package was not found'};" ^
  " $files=@(Get-Item -LiteralPath $requested)" ^
  "}else{" ^
  " $files=Get-ChildItem -LiteralPath $env:OESDK_DOWNLOADS -File -Filter 'OESDK-Complete-*.zip' -ErrorAction SilentlyContinue" ^
  "};" ^
  "$valid=@();" ^
  "foreach($file in $files){" ^
  " $zip=$null;" ^
  " try{" ^
  "  $zip=[IO.Compression.ZipFile]::OpenRead($file.FullName);" ^
  "  $names=@($zip.Entries|ForEach-Object {$_.FullName.Replace('/','\')});" ^
  "  $hasVersion=[bool]($names|Where-Object {$_ -match '(^|\\)(manifest\.json|VERSION)$'}|Select-Object -First 1);" ^
  "  $hasLibC=[bool]($names|Where-Object {$_ -match '(^|\\)(Packages\\LibC|LibC)(\\|$)'}|Select-Object -First 1);" ^
  "  $hasSetup=[bool]($names|Where-Object {$_ -match '(^|\\)Setup\.bat$'}|Select-Object -First 1);" ^
  "  $hasUpdater=[bool]($names|Where-Object {$_ -match '(^|\\)Update\.bat$'}|Select-Object -First 1);" ^
  "  $hasExe=[bool]($names|Where-Object {$_ -match '\.exe$'}|Select-Object -First 1);" ^
  "  if($hasVersion -and $hasLibC -and $hasSetup -and $hasUpdater -and -not $hasExe){" ^
  "   $version=$null;" ^
  "   $manifestEntry=$zip.Entries|Where-Object {$_.FullName -match '(^|/)manifest\.json$'}|Select-Object -First 1;" ^
  "   if($manifestEntry){" ^
  "    $reader=New-Object IO.StreamReader($manifestEntry.Open());" ^
  "    try{$manifest=$reader.ReadToEnd()|ConvertFrom-Json;$candidate=if($manifest.packageRevision){[string]$manifest.packageRevision}else{[string]$manifest.sdkVersion};$version=[version]$candidate}finally{$reader.Dispose()}" ^
  "   };" ^
  "   if(-not $version){" ^
  "    $versionEntry=$zip.Entries|Where-Object {$_.FullName -match '(^|/)VERSION$'}|Select-Object -First 1;" ^
  "    if($versionEntry){$reader=New-Object IO.StreamReader($versionEntry.Open());try{$version=[version]($reader.ReadToEnd().Trim())}finally{$reader.Dispose()}}" ^
  "   };" ^
  "   if($version){$valid += [pscustomobject]@{File=$file;Version=$version}}" ^
  "  }" ^
  " }catch{}finally{if($zip){$zip.Dispose()}}" ^
  "};" ^
  "$selected=$valid|Sort-Object Version -Descending|Select-Object -First 1;" ^
  "if(-not $selected){throw 'No complete script-only OESDK package was found'};" ^
  "[IO.File]::WriteAllText($env:OESDK_PACKAGE_FILE,$selected.File.FullName,[Text.Encoding]::ASCII)"
if errorlevel 1 (
    call :Fail "No valid complete source ZIP named OESDK-Complete-*.zip was found in Downloads."
    goto Cleanup
)

set "SelectedPackage="
set /P "SelectedPackage="<"%PackageFile%"
if not defined SelectedPackage (
    call :Fail "The selected package path could not be read."
    goto Cleanup
)
call :Ok "Selected latest complete source ZIP: %SelectedPackage%."

rem Safe extraction.
set "OESDK_ZIP=%SelectedPackage%"
set "OESDK_DEST=%ExtractRoot%"
powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
  "$ErrorActionPreference='Stop';Add-Type -AssemblyName System.IO.Compression.FileSystem;" ^
  "$root=[IO.Path]::GetFullPath($env:OESDK_DEST);" ^
  "$zip=[IO.Compression.ZipFile]::OpenRead($env:OESDK_ZIP);" ^
  "try{foreach($entry in $zip.Entries){" ^
  " $target=[IO.Path]::GetFullPath((Join-Path $root $entry.FullName));" ^
  " if($target -ne $root -and -not $target.StartsWith($root.TrimEnd('\')+'\',[StringComparison]::OrdinalIgnoreCase)){throw ('Unsafe ZIP path: '+$entry.FullName)}" ^
  "}}finally{$zip.Dispose()};" ^
  "[IO.Compression.ZipFile]::ExtractToDirectory($env:OESDK_ZIP,$root)"
if errorlevel 1 (
    call :Fail "The package could not be safely extracted."
    goto Cleanup
)

set "OESDK_EXTRACT_ROOT=%ExtractRoot%"
set "OESDK_ROOT_FILE=%RootFile%"
powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
  "$items=@(Get-ChildItem -Force -LiteralPath $env:OESDK_EXTRACT_ROOT);" ^
  "$root=$env:OESDK_EXTRACT_ROOT;" ^
  "if($items.Count -eq 1 -and $items[0].PSIsContainer){$root=$items[0].FullName};" ^
  "[IO.File]::WriteAllText($env:OESDK_ROOT_FILE,$root,[Text.Encoding]::ASCII)"
if errorlevel 1 (
    call :Fail "Unable to determine the package root."
    goto Cleanup
)
set /P "PackageRoot="<"%RootFile%"

set "OESDK_PACKAGE_ROOT=%PackageRoot%"
set "OESDK_VERSION_FILE=%VersionFile%"
powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
  "$ErrorActionPreference='Stop';$root=$env:OESDK_PACKAGE_ROOT;$version=$null;" ^
  "$manifest=Join-Path $root 'manifest.json';" ^
  "if(Test-Path -LiteralPath $manifest){$m=Get-Content -Raw -LiteralPath $manifest|ConvertFrom-Json;$version=[string]$m.sdkVersion};" ^
  "if([string]::IsNullOrWhiteSpace($version)){$vf=Join-Path $root 'VERSION';if(Test-Path $vf){$version=(Get-Content -Raw $vf).Trim()}};" ^
  "if($version -notmatch '^[0-9]+\.[0-9]+\.[0-9]+$'){throw 'Invalid SDK version'};" ^
  "foreach($required in @('Update.bat','Setup.bat','Tools\Install-VisualStudio.ps1')){" ^
  " if(-not(Test-Path -LiteralPath (Join-Path $root $required))){throw ('Missing '+$required)}" ^
  "};" ^
  "$lib=Get-ChildItem -LiteralPath $root -Directory -Recurse -ErrorAction SilentlyContinue|Where-Object {$_.FullName -match '\\(Packages\\LibC|LibC)$'}|Select-Object -First 1;" ^
  "if(-not $lib){throw 'LibC is missing'};" ^
  "$exe=Get-ChildItem -LiteralPath $root -File -Filter '*.exe' -Recurse -ErrorAction SilentlyContinue|Select-Object -First 1;" ^
  "if($exe){throw ('Custom executable is not permitted in this package: '+$exe.FullName)};" ^
  "[IO.File]::WriteAllText($env:OESDK_VERSION_FILE,$version,[Text.Encoding]::ASCII)"
if errorlevel 1 (
    call :Fail "The complete package failed validation."
    goto Cleanup
)
set /P "SourceVersion="<"%VersionFile%"
call :Ok "Validated OESDK %SourceVersion% with LibC and full source."

set "BackupRoot=%WorkRoot%\PreviousOESDK"
if exist "%InstallRoot%" robocopy.exe "%InstallRoot%" "%BackupRoot%" /E /COPY:DAT /DCOPY:DAT /R:2 /W:1 /NFL /NDL /NJH /NJS >nul
if exist "%InstallRoot%" rmdir /S /Q "%InstallRoot%"
mkdir "%InstallRoot%" >nul 2>nul
robocopy.exe "%PackageRoot%" "%InstallRoot%" /E /COPY:DAT /DCOPY:DAT /R:2 /W:1 /NFL /NDL /NJH /NJS >nul
if errorlevel 8 (
    call :Fail "Unable to install the complete SDK into C:\OESDK."
    goto RestorePrevious
)
call :Ok "Extracted the full source and LibC to C:\OESDK."

rem Commit exact installed tree to GitHub.
git.exe clone -c core.autocrlf=false -c core.eol=lf "https://github.com/%Repository%.git" "%Checkout%"
if errorlevel 1 (
    call :Fail "Unable to clone the GitHub repository."
    goto Cleanup
)
git.exe -C "%Checkout%" config core.autocrlf false
git.exe -C "%Checkout%" config core.eol lf
git.exe -C "%Checkout%" checkout -B "%Branch%" "origin/%Branch%" >nul 2>nul
if errorlevel 1 (
    call :Fail "Unable to prepare Git branch %Branch%."
    goto Cleanup
)
if exist "%Checkout%\Installer" rmdir /S /Q "%Checkout%\Installer"
mkdir "%Checkout%\Installer" >nul 2>nul
robocopy.exe "%InstallRoot%" "%Checkout%\Installer" /E /COPY:DAT /DCOPY:DAT /R:2 /W:1 /XD ".git" /NFL /NDL /NJH /NJS >nul
if errorlevel 8 (
    call :Fail "Unable to copy the installed SDK into the repository."
    goto Cleanup
)
if not exist "%Checkout%\Installer\.gitattributes" (
    >"%Checkout%\Installer\.gitattributes" echo * text=auto eol=lf
    >>"%Checkout%\Installer\.gitattributes" echo *.bat text eol=crlf
    >>"%Checkout%\Installer\.gitattributes" echo *.cmd text eol=crlf
    >>"%Checkout%\Installer\.gitattributes" echo *.ps1 text eol=crlf
)
rem Resolve a valid Git author before attempting the commit.
rem Priority:
rem   1. GIT_AUTHOR_NAME / GIT_AUTHOR_EMAIL environment variables
rem   2. C:\OESDK\OESDKProjectDefaults.json
rem   3. existing global Git configuration
rem   4. safe OESDK updater identity
set "AuthorName=%GIT_AUTHOR_NAME%"
set "AuthorEmail=%GIT_AUTHOR_EMAIL%"
set "AuthorFile=%WorkRoot%\GitAuthor.txt"
set "OESDK_AUTHOR_FILE=%AuthorFile%"
set "OESDK_DEFAULTS_FILE=%InstallRoot%\OESDKProjectDefaults.json"
set "OESDK_AUTHOR_NAME=%AuthorName%"
set "OESDK_AUTHOR_EMAIL=%AuthorEmail%"

powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
  "$name=$env:OESDK_AUTHOR_NAME;" ^
  "$email=$env:OESDK_AUTHOR_EMAIL;" ^
  "$defaults=$env:OESDK_DEFAULTS_FILE;" ^
  "if((([string]::IsNullOrWhiteSpace($name)) -or ([string]::IsNullOrWhiteSpace($email))) -and (Test-Path -LiteralPath $defaults -PathType Leaf)){" ^
  " try{" ^
  "  $d=Get-Content -Raw -LiteralPath $defaults|ConvertFrom-Json;" ^
  "  if([string]::IsNullOrWhiteSpace($name)){$name=[string]$d.AuthorName};" ^
  "  if([string]::IsNullOrWhiteSpace($email)){$email=[string]$d.AuthorEmail}" ^
  " }catch{}" ^
  "};" ^
  "if([string]::IsNullOrWhiteSpace($name)){$name=(git config --global --get user.name 2>$null|Out-String).Trim()};" ^
  "if([string]::IsNullOrWhiteSpace($email)){$email=(git config --global --get user.email 2>$null|Out-String).Trim()};" ^
  "if([string]::IsNullOrWhiteSpace($name)){$name='OESDK Updater'};" ^
  "if([string]::IsNullOrWhiteSpace($email) -or $email -notmatch '^[^@\s]+@[^@\s]+\.[^@\s]+$'){$email='updater@projectlithos.invalid'};" ^
  "$name=$name.Trim();" ^
  "$email=$email.Trim();" ^
  "[IO.File]::WriteAllLines($env:OESDK_AUTHOR_FILE,@($name,$email),[Text.Encoding]::ASCII)"
if errorlevel 1 (
    call :Fail "Unable to resolve a Git author identity."
    goto Cleanup
)

for /f "usebackq tokens=1,* delims=:" %%A in (`findstr /n "^" "%AuthorFile%"`) do (
    if "%%A"=="1" set "AuthorName=%%B"
    if "%%A"=="2" set "AuthorEmail=%%B"
)

if not defined AuthorName set "AuthorName=OESDK Updater"
if not defined AuthorEmail set "AuthorEmail=updater@projectlithos.invalid"

git.exe -C "%Checkout%" config user.name "%AuthorName%"
if errorlevel 1 (
    call :Fail "Unable to configure the repository Git author name."
    goto Cleanup
)
git.exe -C "%Checkout%" config user.email "%AuthorEmail%"
if errorlevel 1 (
    call :Fail "Unable to configure the repository Git author email."
    goto Cleanup
)
call :Ok "Git author: %AuthorName% <%AuthorEmail%>."

git.exe -C "%Checkout%" add -A -- Installer
git.exe -C "%Checkout%" diff --cached --quiet
if errorlevel 2 (
    call :Fail "Git could not inspect the staged changes."
    goto Cleanup
)
if errorlevel 1 (
    git.exe -C "%Checkout%" commit -m "Update OESDK installer to %SourceVersion%"
    if errorlevel 1 (
        call :Fail "Git commit failed."
        goto Cleanup
    )
    if not defined OESDK_SKIP_PUSH (
        git.exe -C "%Checkout%" push origin "HEAD:%Branch%"
        if errorlevel 1 (
            call :Fail "Git push failed."
            goto Cleanup
        )
        call :Ok "Committed and pushed OESDK %SourceVersion% to GitHub."
    ) else (
        call :Ok "Committed locally; push skipped."
    )
) else (
    call :Ok "GitHub already contains this exact SDK tree."
)

call :Ok "Starting local Visual Studio setup."
call "%InstallRoot%\Setup.bat"
set "SetupExit=%errorlevel%"
if not "%SetupExit%"=="0" (
    call :Fail "Visual Studio setup exited with code %SetupExit%."
    goto Cleanup
)
call :Ok "Visual Studio SDK setup completed."
set "ExitCode=0"
goto Cleanup

:RestorePrevious
if exist "%InstallRoot%" rmdir /S /Q "%InstallRoot%"
if exist "%BackupRoot%" (
    mkdir "%InstallRoot%" >nul 2>nul
    robocopy.exe "%BackupRoot%" "%InstallRoot%" /E /COPY:DAT /DCOPY:DAT /R:2 /W:1 /NFL /NDL /NJH /NJS >nul
)
goto Cleanup

:Ok
echo [ OK ] %~1
exit /b 0

:Fail
echo [FAIL] %~1 1>&2
set "ExitCode=1"
exit /b 0

:Cleanup
if defined WorkRoot if exist "%WorkRoot%" rmdir /S /Q "%WorkRoot%" >nul 2>nul
exit /b %ExitCode%
