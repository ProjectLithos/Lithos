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

function Receive-File {
    param([Uri]$Uri, [string]$Path, $ProgressBar, $StatusLabel)
    $client = New-Object Net.Http.HttpClient
    try {
        $response = $client.GetAsync($Uri, [Net.Http.HttpCompletionOption]::ResponseHeadersRead).Result
        $response.EnsureSuccessStatusCode()
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

function Install-VisualStudioSupport {
    param($Manifest, [string]$InstallRoot, $ProgressBar, $StatusLabel)
    $installerRoot = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer'
    $vswhere = Join-Path $installerRoot 'vswhere.exe'
    if (-not (Test-Path $vswhere)) {
        if (-not $Manifest.visualStudio -or -not $Manifest.visualStudio.communityBootstrapperUrl) {
            throw 'Visual Studio Community was not found and the manifest does not provide its official bootstrapper address.'
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
            $components = @('Microsoft.VisualStudio.Workload.NativeDesktop', 'Microsoft.VisualStudio.Component.VC.Llvm.Clang')
            if ($Manifest.visualStudio.components) { $components = @($Manifest.visualStudio.components) }
            $arguments = @('--passive', '--wait', '--norestart', '--includeRecommended')
            foreach ($component in $components) { $arguments += @('--add', [string]$component) }
            $process = Start-Process -FilePath $bootstrapper -ArgumentList $arguments -Wait -PassThru
            if ($process.ExitCode -notin @(0, 3010)) { throw "Visual Studio Community installation failed with code $($process.ExitCode)." }
        } finally {
            Remove-Item -LiteralPath $bootstrapper -Force -ErrorAction SilentlyContinue
        }
        if (-not (Test-Path $vswhere)) { throw 'Visual Studio Community installation completed but could not be detected.' }
    }

    $vsPath = (& $vswhere -latest -products Microsoft.VisualStudio.Product.Community -property installationPath).Trim()
    if (-not $vsPath) { throw 'Visual Studio Community was not found.' }

    $components = @('Microsoft.VisualStudio.Workload.NativeDesktop', 'Microsoft.VisualStudio.Component.VC.Llvm.Clang')
    if ($Manifest.visualStudio -and $Manifest.visualStudio.components) {
        $components = @($Manifest.visualStudio.components)
    }

    $setup = Join-Path $installerRoot 'setup.exe'
    if (-not (Test-Path $setup)) { throw 'The Visual Studio Installer is incomplete.' }
    $arguments = @('modify', '--installPath', $vsPath, '--passive', '--norestart', '--includeRecommended')
    foreach ($component in $components) { $arguments += @('--add', [string]$component) }
    $StatusLabel.Text = 'Configuring Visual Studio Community...'
    [Windows.Forms.Application]::DoEvents()
    $process = Start-Process -FilePath $setup -ArgumentList $arguments -Wait -PassThru
    if ($process.ExitCode -notin @(0, 3010)) { throw "Visual Studio configuration failed with code $($process.ExitCode)." }

    if ($Manifest.visualStudio -and $Manifest.visualStudio.vsix) {
        $vsixPath = Get-SafeDestination $InstallRoot ([string]$Manifest.visualStudio.vsix)
        $vsixInstaller = Join-Path $vsPath 'Common7\IDE\VSIXInstaller.exe'
        if (-not (Test-Path $vsixPath)) { throw "The Visual Studio extension is missing: $vsixPath" }
        $process = Start-Process -FilePath $vsixInstaller -ArgumentList @('/quiet', $vsixPath) -Wait -PassThru
        if ($process.ExitCode -ne 0) { throw "Visual Studio extension installation failed with code $($process.ExitCode)." }
    }
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
        if ($manifest.schemaVersion -ne 1 -or -not $manifest.packages) { throw 'Unsupported or incomplete OESDK package manifest.' }

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
            [IO.Compression.ZipFile]::ExtractToDirectory($archivePath, $destination)
        }

        $StatusLabel.Text = 'Installing OESDK files...'
        [Windows.Forms.Application]::DoEvents()
        [void][IO.Directory]::CreateDirectory($InstallRoot)
        Copy-Item -Path (Join-Path $stageRoot '*') -Destination $InstallRoot -Recurse -Force
        [Environment]::SetEnvironmentVariable('OESDK_ROOT', $InstallRoot, 'Machine')
        $env:OESDK_ROOT = $InstallRoot

        Install-VisualStudioSupport $manifest $InstallRoot $ProgressBar $StatusLabel
        $ProgressBar.Value = 100
        $StatusLabel.Text = "OESDK $($manifest.sdkVersion) installed successfully."
    } finally {
        if (Test-Path $workRoot) { Remove-Item -LiteralPath $workRoot -Recurse -Force -ErrorAction SilentlyContinue }
    }
}

$form = New-Object Windows.Forms.Form
$form.Text = 'OESDK Setup 0.0.1'
$form.StartPosition = 'CenterScreen'
$form.ClientSize = New-Object Drawing.Size(660, 300)
$form.FormBorderStyle = 'FixedDialog'
$form.MaximizeBox = $false
$form.Font = New-Object Drawing.Font('Segoe UI', 9)

[void](New-Label $form 'OESDK Online Installer' 24 20 610 30)
$form.Controls[$form.Controls.Count - 1].Font = New-Object Drawing.Font('Segoe UI Semibold', 16)
[void](New-Label $form 'The SDK, Clang components and QEMU are downloaded during installation.' 26 58 610)
[void](New-Label $form 'Package manifest:' 26 94 130)
$manifestBox = New-TextBox $form 'https://downloads.oesdk.org/releases/stable/manifest.json' 156 91 470
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
        [Windows.Forms.MessageBox]::Show($form, $_.Exception.Message, 'OESDK Setup', 'OK', 'Error') | Out-Null
    } finally {
        $installButton.Enabled = $true
        $manifestBox.Enabled = $true
        $installBox.Enabled = $true
    }
})

[void]$form.ShowDialog()
