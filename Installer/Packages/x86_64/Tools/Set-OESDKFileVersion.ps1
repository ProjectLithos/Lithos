[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$ProjectRoot,
    [Parameter(Mandatory = $true)][string]$File,
    [Parameter(Mandatory = $true)][string]$Version
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ($Version -notmatch '^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$') {
    throw 'Version must use Major.Minor.Patch form.'
}
$root = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\') + '\'
$path = [IO.Path]::GetFullPath((Join-Path $root $File))
if (-not $path.StartsWith($root, [StringComparison]::OrdinalIgnoreCase)) { throw 'The selected file is outside the project.' }
if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "File not found: $File" }

$versionsPath = Join-Path $root 'OESDKFileVersions.json'
$versions = Get-Content -Raw -LiteralPath $versionsPath | ConvertFrom-Json
$relativePath = $path.Substring($root.Length).Replace('\', '/')
$property = $versions.files.PSObject.Properties[$relativePath]
if ($null -eq $property) { throw "The file is not tracked in OESDKFileVersions.json: $relativePath" }

$content = [IO.File]::ReadAllText($path)
$versionPattern = New-Object Text.RegularExpressions.Regex -ArgumentList '(?m)^(\s*\*\s*File Version:\s*)[^\r\n]+$'
$updated = $versionPattern.Replace($content, ('$1' + $Version), 1)
if ($updated -eq $content) { throw "The file version header was not found: $relativePath" }
$property.Value = $Version
$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText($path, $updated, $utf8)
[IO.File]::WriteAllText($versionsPath, (($versions | ConvertTo-Json -Depth 8) + "`r`n"), $utf8)
Write-Host "[ OK ] $relativePath file version is now $Version."
