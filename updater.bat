@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "ExitCode="

set "Repository=ProjectLithos/Lithos"
set "Branch=main"
if defined LITHOS_REPOSITORY set "Repository=%LITHOS_REPOSITORY%"
if defined LITHOS_BRANCH set "Branch=%LITHOS_BRANCH%"

set "SourceInput=%~1"
if not defined SourceInput if defined LITHOS_SOURCE_URL set "SourceInput=%LITHOS_SOURCE_URL%"
if not defined SourceInput goto Usage

for %%T in (curl.exe git.exe powershell.exe robocopy.exe) do (
    where %%T >nul 2>nul
    if errorlevel 1 (
        call :Fail "Required tool not found: %%T"
        goto Cleanup
    )
)

set "LITHOS_EFFECTIVE_SOURCE_INPUT=%SourceInput%"
set "LITHOS_EFFECTIVE_REPOSITORY=%Repository%"
set "LITHOS_EFFECTIVE_BRANCH=%Branch%"
powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
    "$s=$env:LITHOS_EFFECTIVE_SOURCE_INPUT; if($env:LITHOS_EFFECTIVE_REPOSITORY -notmatch '^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$'){exit 2}; if($env:LITHOS_EFFECTIVE_BRANCH -notmatch '^[A-Za-z0-9._/-]+$'){exit 3}; if(Test-Path -LiteralPath $s -PathType Leaf){if([IO.Path]::GetExtension($s) -ne '.zip'){exit 1}; exit 0}; $u=$null; if(-not [Uri]::TryCreate($s,[UriKind]::Absolute,[ref]$u) -or $u.Scheme -ne 'https'){exit 1}; if($u.AbsolutePath.EndsWith('.git',[StringComparison]::OrdinalIgnoreCase)){exit 4}"
set "ValidationResult=%errorlevel%"
if "%ValidationResult%"=="4" (
    call :Fail "The GitHub repository address is the destination, not a source ZIP. Pass the downloaded source ZIP instead."
    goto Cleanup
)
if "%ValidationResult%"=="3" (
    call :Fail "LITHOS_BRANCH contains unsupported characters."
    goto Cleanup
)
if "%ValidationResult%"=="2" (
    call :Fail "LITHOS_REPOSITORY must use owner/repository form."
    goto Cleanup
)
if not "%ValidationResult%"=="0" (
    call :Fail "Pass an existing source ZIP file or a valid HTTPS source ZIP address."
    goto Cleanup
)

set "WorkRoot=%TEMP%\LithosUpdater-%RANDOM%-%RANDOM%"
set "Archive=%WorkRoot%\Source.zip"
set "Extracted=%WorkRoot%\Extracted"
set "Checkout=%WorkRoot%\Checkout"
mkdir "%Extracted%" >nul 2>nul
if errorlevel 1 (
    call :Fail "Unable to create the temporary workspace."
    goto Cleanup
)

if exist "%SourceInput%" (
    call :Ok "Using local source archive."
    copy /B /Y "%SourceInput%" "%Archive%" >nul
    if errorlevel 1 (
        call :Fail "The local source archive could not be copied."
        goto Cleanup
    )
) else (
    call :Ok "Downloading source archive."
    curl.exe --fail --location --proto "=https" --tlsv1.2 --retry 3 --retry-delay 2 --output "%Archive%" "%SourceInput%"
    if errorlevel 1 (
        call :Fail "The source archive download failed."
        goto Cleanup
    )
)

if defined LITHOS_SOURCE_SHA256 (
    set "LITHOS_DOWNLOADED_ARCHIVE=%Archive%"
    powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
        "$e=$env:LITHOS_SOURCE_SHA256; if($e -notmatch '^[A-Fa-f0-9]{64}$'){exit 2}; $a=(Get-FileHash -Algorithm SHA256 -LiteralPath $env:LITHOS_DOWNLOADED_ARCHIVE).Hash; if($a -ne $e){exit 1}"
    if errorlevel 2 (
        call :Fail "LITHOS_SOURCE_SHA256 must contain exactly 64 hexadecimal characters."
        goto Cleanup
    )
    if errorlevel 1 (
        call :Fail "The source archive SHA-256 does not match."
        goto Cleanup
    )
    call :Ok "Source archive SHA-256 verified."
)

set "LITHOS_DOWNLOADED_ARCHIVE=%Archive%"
set "LITHOS_EXTRACTED_ROOT=%Extracted%"
powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
    "$ErrorActionPreference='Stop'; Add-Type -AssemblyName System.IO.Compression.FileSystem; $d=[IO.Path]::GetFullPath($env:LITHOS_EXTRACTED_ROOT); $z=$null; try {$z=[IO.Compression.ZipFile]::OpenRead($env:LITHOS_DOWNLOADED_ARCHIVE); foreach($e in $z.Entries){$p=[IO.Path]::GetFullPath((Join-Path $d $e.FullName)); if($p -ne $d -and -not $p.StartsWith($d.TrimEnd('\')+'\',[StringComparison]::OrdinalIgnoreCase)){throw ('Unsafe archive entry: '+$e.FullName)}}} finally {if($null -ne $z){$z.Dispose()}}; [IO.Compression.ZipFile]::ExtractToDirectory($env:LITHOS_DOWNLOADED_ARCHIVE,$d)"
if errorlevel 1 (
    call :Fail "The source archive is invalid or contains an unsafe path."
    goto Cleanup
)

for /f "usebackq delims=" %%I in (`powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
    "$i=@(Get-ChildItem -Force -LiteralPath $env:LITHOS_EXTRACTED_ROOT); if($i.Count -eq 1 -and $i[0].PSIsContainer){$i[0].FullName}else{$env:LITHOS_EXTRACTED_ROOT}"`) do set "SourceRoot=%%I"
if not defined SourceRoot (
    call :Fail "The downloaded source archive is empty."
    goto Cleanup
)

call :Ok "Cloning %Repository%."
git.exe clone "https://github.com/%Repository%.git" "%Checkout%"
if errorlevel 1 (
    call :Fail "The GitHub repository could not be cloned."
    goto Cleanup
)

git.exe -C "%Checkout%" show-ref --verify --quiet "refs/remotes/origin/%Branch%"
if not errorlevel 1 (
    git.exe -C "%Checkout%" checkout -B "%Branch%" "origin/%Branch%" >nul 2>nul
) else (
    git.exe -C "%Checkout%" rev-parse --verify HEAD >nul 2>nul
    if errorlevel 1 (
        git.exe -C "%Checkout%" symbolic-ref HEAD "refs/heads/%Branch%"
    ) else (
        git.exe -C "%Checkout%" checkout -b "%Branch%" >nul 2>nul
    )
)
if errorlevel 1 (
    call :Fail "The target Git branch could not be prepared."
    goto Cleanup
)

git.exe -C "%Checkout%" rm -r --ignore-unmatch Installer >nul 2>nul
if exist "%Checkout%\Installer" (
    powershell.exe -NoLogo -NoProfile -NonInteractive -Command "Remove-Item -LiteralPath '%Checkout%\Installer' -Recurse -Force"
    if errorlevel 1 (
        call :Fail "The existing managed Installer directory could not be removed."
        goto Cleanup
    )
)
mkdir "%Checkout%\Installer" >nul 2>nul
if errorlevel 1 (
    call :Fail "The managed Installer directory could not be created."
    goto Cleanup
)

robocopy.exe "%SourceRoot%" "%Checkout%\Installer" /E /COPY:DAT /DCOPY:DAT /R:2 /W:1 /NFL /NDL /NJH /NJS >nul
if errorlevel 8 (
    call :Fail "The downloaded source could not be copied into the repository."
    goto Cleanup
)
copy /Y "%~f0" "%Checkout%\updater.bat" >nul
if errorlevel 1 (
    call :Fail "updater.bat could not be copied into the repository."
    goto Cleanup
)

set "LITHOS_INSTALLER_ROOT=%Checkout%\Installer"
powershell.exe -NoLogo -NoProfile -NonInteractive -Command ^
    "Get-ChildItem -LiteralPath $env:LITHOS_INSTALLER_ROOT -Directory -Filter .git -Recurse -Force -ErrorAction SilentlyContinue | Remove-Item -Recurse -Force"
if errorlevel 1 (
    call :Fail "Embedded Git metadata could not be removed from the source."
    goto Cleanup
)

git.exe -C "%Checkout%" add -A -- Installer updater.bat
git.exe -C "%Checkout%" diff --cached --quiet
if errorlevel 2 (
    call :Fail "Git could not inspect the staged source update."
    goto Cleanup
)
if not errorlevel 1 (
    call :Ok "The repository already contains this source version."
    goto Success
)

set "Version=%LITHOS_VERSION%"
if not defined Version for /f "usebackq delims=" %%I in (`powershell.exe -NoLogo -NoProfile -NonInteractive -Command "[DateTime]::UtcNow.ToString('yyyy.MM.dd-HHmmss')"`) do set "Version=%%I"
set "AuthorName=%GIT_AUTHOR_NAME%"
set "AuthorEmail=%GIT_AUTHOR_EMAIL%"
if not defined AuthorName set "AuthorName=Lithos Updater"
if not defined AuthorEmail set "AuthorEmail=updater@projectlithos.invalid"

git.exe -C "%Checkout%" -c "user.name=%AuthorName%" -c "user.email=%AuthorEmail%" commit -m "Update Lithos source %Version%"
if errorlevel 1 (
    call :Fail "The source update could not be committed."
    goto Cleanup
)
git.exe -C "%Checkout%" push origin "HEAD:%Branch%"
if errorlevel 1 (
    call :Fail "The source update could not be pushed to GitHub."
    goto Cleanup
)
call :Ok "Uploaded all downloaded source to %Repository% on branch %Branch%."

:Success
set "ExitCode=0"
goto Cleanup

:Usage
echo Usage: updater.bat ^<source-zip-file-or-https-zip-url^>
echo Example: updater.bat "C:\Users\daves\Downloads\OESDK-Installer-0.0.1-Source.zip"
echo.
echo Optional variables: LITHOS_SOURCE_SHA256, LITHOS_REPOSITORY,
echo LITHOS_BRANCH, LITHOS_VERSION, GIT_AUTHOR_NAME, GIT_AUTHOR_EMAIL.
exit /b 2

:Ok
echo [ OK ] %~1
exit /b 0

:Fail
echo [FAIL] %~1 1>&2
set "ExitCode=1"
exit /b 0

:Cleanup
if defined WorkRoot if exist "%WorkRoot%" rmdir /s /q "%WorkRoot%"
if not defined ExitCode set "ExitCode=1"
exit /b %ExitCode%
