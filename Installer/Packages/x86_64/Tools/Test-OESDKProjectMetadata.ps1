[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$ProjectRoot)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\') + '\'
$projectPath = Join-Path $root 'OESDKProject.json'
$versionsPath = Join-Path $root 'OESDKFileVersions.json'
if (-not (Test-Path -LiteralPath $projectPath -PathType Leaf)) { throw 'OESDKProject.json is missing.' }
if (-not (Test-Path -LiteralPath $versionsPath -PathType Leaf)) { throw 'OESDKFileVersions.json is missing.' }

$project = Get-Content -Raw -LiteralPath $projectPath | ConvertFrom-Json
$versions = Get-Content -Raw -LiteralPath $versionsPath | ConvertFrom-Json
if ($project.schemaVersion -ne 1 -or $versions.schemaVersion -ne 1) { throw 'Unsupported OESDK project metadata schema.' }
if ([string]::IsNullOrWhiteSpace([string]$project.projectName)) { throw 'The project name is missing from OESDKProject.json.' }
if ([string]::IsNullOrWhiteSpace([string]$project.author.name)) { throw 'The author name is missing from OESDKProject.json.' }
if ([string]::IsNullOrWhiteSpace([string]$project.author.email)) { throw 'The author email is missing from OESDKProject.json.' }
if ([string]::IsNullOrWhiteSpace([string]$project.license)) { throw 'The OS licence is missing from OESDKProject.json.' }
if ([string]$project.osVersion -notmatch '^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$') {
    throw 'The OS version must use Major.Minor.Patch form.'
}

$entries = @($versions.files.PSObject.Properties)
if ($entries.Count -eq 0) { throw 'OESDKFileVersions.json does not contain any tracked files.' }
foreach ($entry in $entries) {
    $relativePath = [string]$entry.Name
    $version = [string]$entry.Value
    if ($version -notmatch '^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$') {
        throw "Invalid file version for ${relativePath}: $version"
    }
    $path = [IO.Path]::GetFullPath((Join-Path $root $relativePath))
    if (-not $path.StartsWith($root, [StringComparison]::OrdinalIgnoreCase)) { throw "Unsafe tracked file path: $relativePath" }
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Tracked file is missing: $relativePath" }
    $content = [IO.File]::ReadAllText($path)
    $commentEnd = $content.IndexOf('*/')
    if ($commentEnd -lt 0) { throw "The metadata header comment is missing: $relativePath" }
    $header = $content.Substring(0, $commentEnd + 2)
    foreach ($requiredLine in @(
        "Project: $($project.projectName)",
        "File Version: $version",
        "Author: $($project.author.name)",
        "Email: $($project.author.email)",
        "License: $($project.license)",
        "SPDX-License-Identifier: $($project.license)"
    )) {
        if (-not $header.Contains($requiredLine)) { throw "Metadata header mismatch in ${relativePath}: $requiredLine" }
    }
    if ($header -match '(?i)OS\s+Version\s*:') { throw "The OS version must not appear in the file header: $relativePath" }
}

$osVersionHeader = Join-Path $root 'Include\OSVersion.h'
$osVersionText = [IO.File]::ReadAllText($osVersionHeader)
if (-not $osVersionText.Contains('#define OESDK_OS_VERSION_TEXT "' + [string]$project.osVersion + '"')) {
    throw 'OSVersion.h does not match OESDKProject.json.'
}

Write-Host "[ OK ] Project metadata and $($entries.Count) file versions verified."
