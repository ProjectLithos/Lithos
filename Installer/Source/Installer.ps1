Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.IO.Compression.FileSystem
Add-Type -AssemblyName System.Net.Http

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

function Add-Control {
    param($Form, $Control)
    [void]$Form.Controls.Add($Control)
    return $Control
}

function New-Label {
    param($Form, [string]$Text, [int]$X, [int]$Y, [int]$Width, [int]$Height = 22)
    $label = New-Object Windows.Forms.Label
    $label.Text = $Text
    $label.Location = New-Object Drawing.Point($X, $Y)
    $label.Size = New-Object Drawing.Size($Width, $Height)
    return Add-Control $Form $label
}

function New-TextBox {
    param($Form, [string]$Text, [int]$X, [int]$Y, [int]$Width)
    $box = New-Object Windows.Forms.TextBox
    $box.Text = $Text
    $box.Location = New-Object Drawing.Point($X, $Y)
    $box.Size = New-Object Drawing.Size($Width, 24)
    return Add-Control $Form $box
}

function Get-HttpsUri {
    param([string]$Value)
    $uri = $null
    if (-not [Uri]::TryCreate($Value, [UriKind]::Absolute, [ref]$uri) -or $uri.Scheme -ne 'https') {
        throw 'The package manifest must use a valid HTTPS address.'
    }
    return $uri
}

function Get-SafeDestination {
    param([string]$Root, [string]$RelativePath)
    if ([string]::IsNullOrWhiteSpace($RelativePath)) { $RelativePath = '.' }
    $rootPath = [IO.Path]::GetFullPath($Root).TrimEnd('\') + '\'
    $targetPath = [IO.Path]::GetFullPath((Join-Path $Root $RelativePath))
    if (-not ($targetPath + '\').StartsWith($rootPath, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Unsafe package destination: $RelativePath"
    }
    return $targetPath
}

function Expand-SafeArchive {
    param([string]$ArchivePath, [string]$Destination)
    $root = [IO.Path]::GetFullPath($Destination).TrimEnd('\') + '\'
    $archive = $null
    try {
        $archive = [IO.Compression.ZipFile]::OpenRead($ArchivePath)
        foreach ($entry in $archive.Entries) {
            $entryPath = [IO.Path]::GetFullPath((Join-Path $Destination $entry.FullName))
            if (-not ($entryPath + '\').StartsWith($root, [StringComparison]::OrdinalIgnoreCase)) {
                throw "Unsafe path in package archive: $($entry.FullName)"
            }
        }
    } finally {
        if ($null -ne $archive) { $archive.Dispose() }
    }
    [IO.Compression.ZipFile]::ExtractToDirectory($ArchivePath, $Destination)
}

function Receive-File {
    param([Uri]$Uri, [string]$Path, $ProgressBar, $StatusLabel)
    $client = New-Object Net.Http.HttpClient
    try {
        $response = $client.GetAsync($Uri, [Net.Http.HttpCompletionOption]::ResponseHeadersRead).Result
        [void]$response.EnsureSuccessStatusCode()
        $total = $response.Content.Headers.ContentLength
        $inputStream = $response.Content.ReadAsStreamAsync().Result
        $outputStream = [IO.File]::Create($Path)
        try {
            $buffer = New-Object byte[] 65536
            [long]$received = 0
            while (($count = $inputStream.Read($buffer, 0, $buffer.Length)) -gt 0) {
                $outputStream.Write($buffer, 0, $count)
                $received += $count
                if ($total -and $total -gt 0) {
                    $ProgressBar.Value = [Math]::Min(100, [int](($received * 100) / $total))
                    $StatusLabel.Text = "Downloading: $([Math]::Round($received / 1MB, 1)) of $([Math]::Round($total / 1MB, 1)) MB"
                }
                [Windows.Forms.Application]::DoEvents()
            }
        } finally {
            if ($outputStream) { $outputStream.Dispose() }
            if ($inputStream) { $inputStream.Dispose() }
        }
    } finally {
        $client.Dispose()
    }
}

function Get-VisualStudioInstallation {
    param([string]$VsWhere)
    if (-not (Test-Path $VsWhere)) { return $null }

    $allowedProducts = @(
        'Microsoft.VisualStudio.Product.Community',
        'Microsoft.VisualStudio.Product.Professional',
        'Microsoft.VisualStudio.Product.Enterprise'
    )
    try {
        $json = (& $VsWhere -all -products * -format json -utf8 2>$null | Out-String).Trim()
        if ([string]::IsNullOrWhiteSpace($json)) { return $null }
        $instances = @($json | ConvertFrom-Json)
        return $instances |
            Where-Object {
                $allowedProducts -contains $_.productId -and
                $_.isComplete -ne $false -and $_.isLaunchable -ne $false
            } |
            Sort-Object { [Version]$_.installationVersion } -Descending |
            Select-Object -First 1
    } catch {
        throw "Visual Studio detection failed: $($_.Exception.Message)"
    }
}

function Ensure-VisualStudioSupport {
    param($Manifest, $ProgressBar, $StatusLabel)
    $installerRoot = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer'
    $vswhere = Join-Path $installerRoot 'vswhere.exe'
    $components = @('Microsoft.VisualStudio.Workload.NativeDesktop', 'Microsoft.VisualStudio.Component.VC.Llvm.Clang')
    if ($Manifest.visualStudio -and $Manifest.visualStudio.components) {
        $components = @($Manifest.visualStudio.components)
    }

    $installation = Get-VisualStudioInstallation $vswhere

    if (-not $installation) {
        if (-not $Manifest.visualStudio -or -not $Manifest.visualStudio.communityBootstrapperUrl) {
            throw 'No Visual Studio edition is installed and the manifest does not provide the official Community bootstrapper address.'
        }
        $bootstrapperUri = Get-HttpsUri ([string]$Manifest.visualStudio.communityBootstrapperUrl)
        $bootstrapper = Join-Path $env:TEMP ("OESDK-vs-community-" + [Guid]::NewGuid().ToString('N') + '.exe')
        try {
            $StatusLabel.Text = 'Downloading Visual Studio Community...'
            Receive-File $bootstrapperUri $bootstrapper $ProgressBar $StatusLabel
            $signature = Get-AuthenticodeSignature -LiteralPath $bootstrapper
            if ($signature.Status -ne 'Valid' -or $signature.SignerCertificate.Subject -notmatch 'Microsoft Corporation') {
                throw 'The Visual Studio Community bootstrapper is not validly signed by Microsoft.'
            }
            $arguments = @('--passive', '--wait', '--norestart', '--includeRecommended')
            foreach ($component in $components) { $arguments += @('--add', [string]$component) }
            $StatusLabel.Text = 'Installing Visual Studio Community...'
            [Windows.Forms.Application]::DoEvents()
            $process = Start-Process -FilePath $bootstrapper -ArgumentList $arguments -Wait -PassThru
            if ($process.ExitCode -notin @(0, 3010)) { throw "Visual Studio Community installation failed with code $($process.ExitCode)." }
        } finally {
            Remove-Item -LiteralPath $bootstrapper -Force -ErrorAction SilentlyContinue
        }
        if (-not (Test-Path $vswhere)) { throw 'Visual Studio Community installation completed but the Visual Studio detector is missing.' }
        $installation = Get-VisualStudioInstallation $vswhere
        if (-not $installation) { throw 'Visual Studio Community installation completed but no full Visual Studio edition could be detected.' }
    }

    $vsPath = ([string]$installation.installationPath).Trim()
    $displayName = if ([string]::IsNullOrWhiteSpace([string]$installation.displayName)) { 'Visual Studio' } else { [string]$installation.displayName }
    $setup = Join-Path $installerRoot 'setup.exe'
    if (-not (Test-Path $setup)) { throw 'The Visual Studio Installer is incomplete.' }
    $quotedVsPath = '"' + $vsPath + '"'
    $arguments = @('modify', '--installPath', $quotedVsPath, '--passive', '--norestart', '--includeRecommended')
    foreach ($component in $components) { $arguments += @('--add', [string]$component) }
    $StatusLabel.Text = "Configuring $displayName..."
    [Windows.Forms.Application]::DoEvents()
    $process = Start-Process -FilePath $setup -ArgumentList $arguments -Wait -PassThru
    if ($process.ExitCode -notin @(0, 3010)) { throw "Visual Studio configuration failed with code $($process.ExitCode)." }
    return $vsPath
}

function Remove-OESDKVisualStudioSupport {
    param([string]$VsPath, $StatusLabel)
    $vsixInstaller = Join-Path $VsPath 'Common7\IDE\VSIXInstaller.exe'
    if (-not (Test-Path $vsixInstaller)) { throw "The Visual Studio extension installer is missing: $vsixInstaller" }
    if (Get-Process -Name devenv -ErrorAction SilentlyContinue) {
        throw 'Close every Visual Studio window, then run OESDK Setup again. Visual Studio must be closed while every older OESDK template is removed.'
    }

    $StatusLabel.Text = 'Removing every previous OESDK Visual Studio registration...'
    [Windows.Forms.Application]::DoEvents()
    $extensionIds = New-Object 'System.Collections.Generic.HashSet[string]' ([StringComparer]::OrdinalIgnoreCase)
    [void]$extensionIds.Add('ProjectLithos.OESDK.VisualStudio')
    $extensionRoot = Join-Path $env:LOCALAPPDATA 'Microsoft\VisualStudio'
    if (Test-Path $extensionRoot) {
        foreach ($manifestPath in Get-ChildItem -LiteralPath $extensionRoot -Filter 'extension.vsixmanifest' -File -Recurse -ErrorAction SilentlyContinue) {
            try {
                $text = [IO.File]::ReadAllText($manifestPath.FullName)
                if ($text -match '(?i)OESDK' -and $text -match '(?i)<Identity\s+[^>]*Id="([^"]+)"') {
                    [void]$extensionIds.Add($Matches[1])
                }
            } catch { }
        }
    }
    foreach ($extensionId in $extensionIds) {
        $uninstall = Start-Process -FilePath $vsixInstaller -ArgumentList @('/quiet', "/uninstall:$extensionId") -Wait -PassThru
        # A nonzero result also means that this older identity was not installed.
    }

    $documents = [Environment]::GetFolderPath('MyDocuments')
    if ([string]::IsNullOrWhiteSpace($documents)) { throw 'The Windows Documents folder could not be located.' }
    $projectTemplatesRoot = Join-Path $documents 'Visual Studio 2022\Templates\ProjectTemplates'
    $ownedTemplateRoot = Join-Path $projectTemplatesRoot 'OESDK'
    if (Test-Path -LiteralPath $ownedTemplateRoot) {
        Remove-Item -LiteralPath $ownedTemplateRoot -Recurse -Force
    }
    if (Test-Path -LiteralPath $projectTemplatesRoot) {
        foreach ($oldTemplate in Get-ChildItem -LiteralPath $projectTemplatesRoot -Filter '*OESDK*.zip' -File -Recurse -ErrorAction SilentlyContinue) {
            Remove-Item -LiteralPath $oldTemplate.FullName -Force
        }
    }
}

function Install-VisualStudioExtension {
    param($Manifest, [string]$InstallRoot, [string]$VsPath, $StatusLabel)
    if ($Manifest.visualStudio) {

        if (-not $Manifest.visualStudio.projectTemplates -or @($Manifest.visualStudio.projectTemplates).Count -ne 2) {
            throw 'The OESDK manifest must specify exactly two Visual Studio project templates.'
        }

        $StatusLabel.Text = 'Installing the two OESDK 0.0.8 native Clang C templates...'
        [Windows.Forms.Application]::DoEvents()
        $documents = [Environment]::GetFolderPath('MyDocuments')
        $templateRoot = Join-Path $documents 'Visual Studio 2022\Templates\ProjectTemplates\OESDK'
        [void][IO.Directory]::CreateDirectory($templateRoot)
        foreach ($relativeTemplate in @($Manifest.visualStudio.projectTemplates)) {
            $sourceTemplate = Get-SafeDestination $InstallRoot ([string]$relativeTemplate)
            if (-not (Test-Path -LiteralPath $sourceTemplate)) { throw "The OESDK project template is missing: $sourceTemplate" }
            $templateFile = Join-Path $templateRoot ([IO.Path]::GetFileName($sourceTemplate))
            Copy-Item -LiteralPath $sourceTemplate -Destination $templateFile -Force
        }

        $devenv = Join-Path $VsPath 'Common7\IDE\devenv.com'
        if (Test-Path $devenv) {
            $StatusLabel.Text = 'Rebuilding the Visual Studio project-template cache...'
            [Windows.Forms.Application]::DoEvents()
            $refresh = Start-Process -FilePath $devenv -ArgumentList @('/installvstemplates') -Wait -PassThru -WindowStyle Hidden
            if ($refresh.ExitCode -ne 0) { throw "Visual Studio template refresh failed with code $($refresh.ExitCode)." }
        }
    }
}

function Ensure-QemuSupport {
    param($Manifest, [string]$InstallRoot, $StatusLabel)
    if (-not $Manifest.qemu -or -not $Manifest.qemu.ensureScript) { return }
    $scriptPath = Get-SafeDestination $InstallRoot ([string]$Manifest.qemu.ensureScript)
    if (-not (Test-Path $scriptPath)) { throw "The QEMU setup script is missing: $scriptPath" }
    $StatusLabel.Text = 'Checking QEMU installation...'
    [Windows.Forms.Application]::DoEvents()
    $quotedScript = '"' + $scriptPath + '"'
    $process = Start-Process -FilePath 'powershell.exe' -ArgumentList @(
        '-NoLogo', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', $quotedScript
    ) -Wait -PassThru
    if ($process.ExitCode -ne 0) { throw "QEMU setup failed with code $($process.ExitCode)." }
}

function Install-OESDK {
    param([Uri]$ManifestUri, [string]$InstallRoot, $ProgressBar, $StatusLabel)
    $workRoot = Join-Path $env:TEMP ("OESDK-Setup-" + [Guid]::NewGuid().ToString('N'))
    $downloadRoot = Join-Path $workRoot 'Downloads'
    $stageRoot = Join-Path $workRoot 'Stage'
    [void][IO.Directory]::CreateDirectory($downloadRoot)
    [void][IO.Directory]::CreateDirectory($stageRoot)

    try {
        $StatusLabel.Text = 'Downloading package manifest...'
        $manifestPath = Join-Path $workRoot 'manifest.json'
        Receive-File $ManifestUri $manifestPath $ProgressBar $StatusLabel
        $manifest = Get-Content -Raw -LiteralPath $manifestPath | ConvertFrom-Json
        if ($manifest.schemaVersion -ne 1) { throw 'Unsupported OESDK package manifest.' }
        if ($manifest.releaseStatus -and $manifest.releaseStatus -ne 'ready') {
            $releaseMessage = [string]$manifest.releaseMessage
            if ([string]::IsNullOrWhiteSpace($releaseMessage)) {
                $releaseMessage = 'The OESDK packages have not been published yet.'
            }
            throw $releaseMessage
        }
        if (-not $manifest.packages -or @($manifest.packages).Count -eq 0) {
            throw 'The OESDK manifest does not contain any downloadable packages.'
        }

        $packageIndex = 0
        foreach ($package in @($manifest.packages)) {
            $packageIndex++
            if (-not $package.name -or -not $package.url -or -not $package.sha256) { throw 'A package manifest entry is incomplete.' }
            $packageUri = Get-HttpsUri ([string]$package.url)
            $archivePath = Join-Path $downloadRoot ("Package-$packageIndex.zip")
            $StatusLabel.Text = "Downloading $($package.name)..."
            $ProgressBar.Value = 0
            Receive-File $packageUri $archivePath $ProgressBar $StatusLabel

            $actualHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $archivePath).Hash
            if ($actualHash -ne ([string]$package.sha256).ToUpperInvariant()) {
                throw "Security check failed for $($package.name): SHA-256 does not match."
            }
            $destination = Get-SafeDestination $stageRoot ([string]$package.destination)
            [void][IO.Directory]::CreateDirectory($destination)
            Expand-SafeArchive $archivePath $destination
        }

        $bootstrapVisualStudioManifest = [PSCustomObject]@{
            visualStudio = [PSCustomObject]@{
                communityBootstrapperUrl = 'https://aka.ms/vs/17/release/vs_community.exe'
                components = @(
                    'Microsoft.VisualStudio.Workload.NativeDesktop',
                    'Microsoft.VisualStudio.Component.VC.Llvm.Clang',
                    'Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Llvm.Clang'
                )
            }
        }
        $visualStudioPath = Ensure-VisualStudioSupport $bootstrapVisualStudioManifest $ProgressBar $StatusLabel

        Remove-OESDKVisualStudioSupport $visualStudioPath $StatusLabel

        $installRootPath = [IO.Path]::GetFullPath($InstallRoot)
        $installDriveRoot = [IO.Path]::GetPathRoot($installRootPath)
        if ($installRootPath.TrimEnd('\') -eq $installDriveRoot.TrimEnd('\')) {
            throw 'OESDK cannot clean an installation located at the root of a drive.'
        }
        $StatusLabel.Text = 'Removing the previous OESDK installation...'
        [Windows.Forms.Application]::DoEvents()
        [Environment]::SetEnvironmentVariable('OESDK_ROOT', $null, 'Machine')
        $env:OESDK_ROOT = $null
        if (Test-Path -LiteralPath $installRootPath) {
            $oldInstall = Get-Item -LiteralPath $installRootPath -Force
            if (($oldInstall.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
                throw "OESDK will not remove an installation directory that is a link or reparse point: $installRootPath"
            }
            Remove-Item -LiteralPath $installRootPath -Recurse -Force
        }

        $StatusLabel.Text = 'Installing OESDK files...'
        [Windows.Forms.Application]::DoEvents()
        [void][IO.Directory]::CreateDirectory($InstallRoot)
        Copy-Item -Path (Join-Path $stageRoot '*') -Destination $InstallRoot -Recurse -Force
        [Environment]::SetEnvironmentVariable('OESDK_ROOT', $InstallRoot, 'Machine')
        $env:OESDK_ROOT = $InstallRoot

        Ensure-QemuSupport $manifest $InstallRoot $StatusLabel
        Install-VisualStudioExtension $manifest $InstallRoot $visualStudioPath $StatusLabel
        $ProgressBar.Value = 100
        $StatusLabel.Text = "OESDK $($manifest.sdkVersion) installed successfully."
    } finally {
        if (Test-Path $workRoot) { Remove-Item -LiteralPath $workRoot -Recurse -Force -ErrorAction SilentlyContinue }
    }
}

$form = New-Object Windows.Forms.Form
$form.Text = 'OESDK Setup 0.0.8'
$form.StartPosition = 'CenterScreen'
$form.ClientSize = New-Object Drawing.Size(660, 300)
$form.FormBorderStyle = 'FixedDialog'
$form.MaximizeBox = $false
$form.Font = New-Object Drawing.Font('Segoe UI', 9)

[void](New-Label $form 'OESDK Online Installer' 24 20 610 30)
$form.Controls[$form.Controls.Count - 1].Font = New-Object Drawing.Font('Segoe UI Semibold', 16)
[void](New-Label $form 'The SDK, Clang components and QEMU are downloaded during installation.' 26 58 610)
[void](New-Label $form 'Package manifest:' 26 94 130)
$manifestBox = New-TextBox $form 'https://raw.githubusercontent.com/ProjectLithos/Lithos/main/Installer/manifest.json' 156 91 470
[void](New-Label $form 'Install location:' 26 132 130)
$installBox = New-TextBox $form 'C:\OESDK' 156 129 470

$progress = New-Object Windows.Forms.ProgressBar
$progress.Location = New-Object Drawing.Point(26, 177)
$progress.Size = New-Object Drawing.Size(600, 22)
[void]$form.Controls.Add($progress)
$status = New-Label $form 'Ready to download OESDK.' 26 207 600

$installButton = New-Object Windows.Forms.Button
$installButton.Text = 'Install'
$installButton.Location = New-Object Drawing.Point(438, 252)
$installButton.Size = New-Object Drawing.Size(90, 30)
[void]$form.Controls.Add($installButton)
$cancelButton = New-Object Windows.Forms.Button
$cancelButton.Text = 'Cancel'
$cancelButton.Location = New-Object Drawing.Point(536, 252)
$cancelButton.Size = New-Object Drawing.Size(90, 30)
[void]$form.Controls.Add($cancelButton)
$cancelButton.Add_Click({ $form.Close() })

$installButton.Add_Click({
    $installButton.Enabled = $false
    $manifestBox.Enabled = $false
    $installBox.Enabled = $false
    try {
        $uri = Get-HttpsUri $manifestBox.Text.Trim()
        $root = [IO.Path]::GetFullPath($installBox.Text.Trim())
        if ($root.TrimEnd('\') -eq [IO.Path]::GetPathRoot($root).TrimEnd('\')) {
            throw 'OESDK cannot be installed directly into the root of a drive.'
        }
        Install-OESDK $uri $root $progress $status
        [Windows.Forms.MessageBox]::Show($form, 'OESDK was installed successfully.', 'OESDK Setup', 'OK', 'Information') | Out-Null
    } catch {
        $status.Text = 'Installation failed.'
        $logPath = Join-Path $env:TEMP 'OESDK-Setup.log'
        $details = "OESDK Setup 0.0.8`r`n$([DateTime]::Now.ToString('O'))`r`n$($_ | Out-String)"
        [IO.File]::WriteAllText($logPath, $details)
        $message = "$($_.Exception.Message)`r`n`r`nDiagnostic log: $logPath"
        [Windows.Forms.MessageBox]::Show($form, $message, 'OESDK Setup', 'OK', 'Error') | Out-Null
    } finally {
        $installButton.Enabled = $true
        $manifestBox.Enabled = $true
        $installBox.Enabled = $true
    }
})

[void]$form.ShowDialog()
