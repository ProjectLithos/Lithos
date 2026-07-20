[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ProjectName,

    [ValidateSet('Kernel', 'Desktop')]
    [string]$ProjectType = 'Kernel',

    [string]$Destination = (Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'OESDK Projects'),

    [string]$AuthorName,
    [string]$AuthorEmail,

    [ValidateSet('MIT', 'Apache-2.0', 'BSD-3-Clause', 'GPL-3.0-only', 'GPL-3.0-or-later',
                 'LGPL-3.0-only', 'LGPL-3.0-or-later', 'MPL-2.0', 'CC0-1.0',
                 'LicenseRef-Proprietary', 'LicenseRef-Custom')]
    [string]$License,

    [string]$OsVersion,
    [string]$FileVersion = '0.0.1'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ($ProjectName -notmatch '^[A-Za-z_][A-Za-z0-9_.-]*$') {
    throw 'ProjectName must begin with a letter or underscore and contain only letters, numbers, dots, underscores, or hyphens.'
}

function Get-MetadataDefault {
    param([string]$Name)
    $value = [Environment]::GetEnvironmentVariable($Name, 'Machine')
    if ([string]::IsNullOrWhiteSpace($value)) { $value = [Environment]::GetEnvironmentVariable($Name, 'Process') }
    return $value
}

if ([string]::IsNullOrWhiteSpace($AuthorName)) { $AuthorName = Get-MetadataDefault 'OESDK_AUTHOR_NAME' }
if ([string]::IsNullOrWhiteSpace($AuthorName)) { $AuthorName = Read-Host "Author's name" }
if ([string]::IsNullOrWhiteSpace($AuthorEmail)) { $AuthorEmail = Get-MetadataDefault 'OESDK_AUTHOR_EMAIL' }
if ([string]::IsNullOrWhiteSpace($AuthorEmail)) { $AuthorEmail = Read-Host 'Author email' }
if ([string]::IsNullOrWhiteSpace($License)) { $License = Get-MetadataDefault 'OESDK_LICENSE' }
if ([string]::IsNullOrWhiteSpace($License)) { $License = Read-Host 'OS licence SPDX identifier (for example MIT)' }
if ([string]::IsNullOrWhiteSpace($OsVersion)) { $OsVersion = Get-MetadataDefault 'OESDK_OS_VERSION' }
if ([string]::IsNullOrWhiteSpace($OsVersion)) { $OsVersion = Read-Host 'Initial OS version (Major.Minor.Patch)' }

if ([string]::IsNullOrWhiteSpace($AuthorName) -or $AuthorName -match '[\r\n]' -or $AuthorName.Contains('*/')) {
    throw 'Enter a valid single-line author name.'
}
try { $parsedAddress = New-Object Net.Mail.MailAddress -ArgumentList $AuthorEmail } catch { throw 'Enter a valid author email address.' }
if ($parsedAddress.Address -ne $AuthorEmail -or $AuthorEmail.Contains('*/')) { throw 'Enter one valid author email address.' }
$supportedLicenses = @(
    'MIT', 'Apache-2.0', 'BSD-3-Clause', 'GPL-3.0-only', 'GPL-3.0-or-later',
    'LGPL-3.0-only', 'LGPL-3.0-or-later', 'MPL-2.0', 'CC0-1.0',
    'LicenseRef-Proprietary', 'LicenseRef-Custom'
)
if ($License -notin $supportedLicenses) { throw "Unsupported OS licence: $License" }
foreach ($versionValue in @($OsVersion, $FileVersion)) {
    if ($versionValue -notmatch '^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$') {
        throw "Version '$versionValue' must use Major.Minor.Patch form."
    }
}

$destinationRoot = [IO.Path]::GetFullPath($Destination)
$projectRoot = Join-Path $destinationRoot $ProjectName
if (Test-Path -LiteralPath $projectRoot) {
    if (@(Get-ChildItem -LiteralPath $projectRoot -Force).Count -ne 0) {
        throw "The project folder already exists and is not empty: $projectRoot"
    }
} else {
    [void][IO.Directory]::CreateDirectory($projectRoot)
}
[void][IO.Directory]::CreateDirectory((Join-Path $projectRoot 'Source'))
[void][IO.Directory]::CreateDirectory((Join-Path $projectRoot 'Include'))

$escapedName = [Security.SecurityElement]::Escape($ProjectName)
$projectGuid = [Guid]::NewGuid().ToString('B').ToUpperInvariant()
$projectXml = @'
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64"><Configuration>Debug</Configuration><Platform>x64</Platform></ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64"><Configuration>Release</Configuration><Platform>x64</Platform></ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>__PROJECT_GUID__</ProjectGuid>
    <Keyword>MakeFileProj</Keyword>
    <RootNamespace>__PROJECT_NAME__</RootNamespace>
    <ProjectName>__PROJECT_NAME__</ProjectName>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Makefile</ConfigurationType><PlatformToolset>v143</PlatformToolset><UseDebugLibraries>true</UseDebugLibraries>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Makefile</ConfigurationType><PlatformToolset>v143</PlatformToolset><UseDebugLibraries>false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <NMakeBuildCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Build-Kernel.ps1&quot; -ProjectRoot &quot;$(ProjectDir).&quot; -Configuration &quot;$(Configuration)&quot; -ProjectType __PROJECT_TYPE__</NMakeBuildCommandLine>
    <NMakeReBuildCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Build-Kernel.ps1&quot; -ProjectRoot &quot;$(ProjectDir).&quot; -Configuration &quot;$(Configuration)&quot; -ProjectType __PROJECT_TYPE__ -Clean; powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Build-Kernel.ps1&quot; -ProjectRoot &quot;$(ProjectDir).&quot; -Configuration &quot;$(Configuration)&quot; -ProjectType __PROJECT_TYPE__</NMakeReBuildCommandLine>
    <NMakeCleanCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Build-Kernel.ps1&quot; -ProjectRoot &quot;$(ProjectDir).&quot; -Configuration &quot;$(Configuration)&quot; -ProjectType __PROJECT_TYPE__ -Clean</NMakeCleanCommandLine>
    <NMakeOutput>$(ProjectDir)Build\$(Configuration)\kernel.elf</NMakeOutput>
    <IncludePath>$(OESDK_ROOT)\Include;$(ProjectDir)Include;$(IncludePath)</IncludePath>
    <NMakeIncludeSearchPath>$(OESDK_ROOT)\Include;$(ProjectDir)Include</NMakeIncludeSearchPath>
    <NMakePreprocessorDefinitions Condition="'$(Configuration)'=='Debug'">__INTELLISENSE__=1;DEBUG=1__PROJECT_DEFINITION__</NMakePreprocessorDefinitions>
    <NMakePreprocessorDefinitions Condition="'$(Configuration)'=='Release'">__INTELLISENSE__=1__PROJECT_DEFINITION__</NMakePreprocessorDefinitions>
    <LocalDebuggerCommand>powershell.exe</LocalDebuggerCommand>
    <LocalDebuggerCommandArguments>-NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Run-Qemu.ps1&quot; -Kernel &quot;$(ProjectDir)Build\$(Configuration)\kernel.elf&quot;</LocalDebuggerCommandArguments>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <ItemGroup><ClCompile Include="Source\kmain.c" /></ItemGroup>
  <ItemGroup><ClInclude Include="Include\OSVersion.h" /></ItemGroup>
  <ItemGroup>
    <None Include="AUTHORS" />
    <None Include="LICENSE" />
    <None Include="OESDKFileVersions.json" />
    <None Include="OESDKProject.json" />
    <None Include="README.md" />
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
'@
$projectDefinition = if ($ProjectType -eq 'Desktop') { ';OESDK_DESKTOP=1' } else { '' }
$projectXml = $projectXml.Replace('__PROJECT_GUID__', $projectGuid).Replace('__PROJECT_NAME__', $escapedName).Replace('__PROJECT_TYPE__', $ProjectType).Replace('__PROJECT_DEFINITION__', $projectDefinition)

$fileHeader = @"
/*
 * Project: $ProjectName
 * File: kmain.c
 * File Version: $FileVersion
 * Author: $AuthorName
 * Email: $AuthorEmail
 * License: $License
 * SPDX-License-Identifier: $License
 */

"@

if ($ProjectType -eq 'Desktop') {
    $source = @'
#include <oesdk/kernel.h>
#include "../Include/OSVersion.h"

void kmain(int argc, char *argv)
{
    (void)argc;
    (void)argv;
    kdebugf("__PROJECT_NAME__: starting graphical desktop.\n");

    if (!kgfx_available()) {
        kprintf("A graphics surface is not available.\n");
        return;
    }

    const kgfx_surface *screen = kgfx_screen();
    kgfx_fill(0x183153);
    kgfx_rectangle(0, screen->height - 48, screen->width, 48, 0x101820);
    kgfx_rectangle(48, 48, screen->width > 96 ? screen->width - 96 : 0,
                   screen->height > 144 ? screen->height - 144 : 0, 0xE8EDF2);
    kgfx_rectangle(48, 48, screen->width > 96 ? screen->width - 96 : 0, 36, 0x2878B8);
}
'@
} else {
    $source = @'
#include <oesdk/kernel.h>
#include "../Include/OSVersion.h"

void kmain(int argc, char *argv)
{
    (void)argc;
    (void)argv;

    kdebugf("__PROJECT_NAME__: Hello from the serial debug port!\n");
    kprintf("__PROJECT_NAME__: Hello Screen!\n");
}
'@
}
$source = $fileHeader + $source.Replace('__PROJECT_NAME__', $ProjectName)

$osParts = $OsVersion.Split('.')
$versionHeader = @"
/*
 * Project: $ProjectName
 * File: OSVersion.h
 * File Version: $FileVersion
 * Author: $AuthorName
 * Email: $AuthorEmail
 * License: $License
 * SPDX-License-Identifier: $License
 */

#ifndef OESDK_OS_VERSION_H
#define OESDK_OS_VERSION_H

#define OESDK_OS_VERSION_MAJOR $($osParts[0])
#define OESDK_OS_VERSION_MINOR $($osParts[1])
#define OESDK_OS_VERSION_PATCH $($osParts[2])
#define OESDK_OS_VERSION_TEXT "$OsVersion"

#endif
"@

$projectMetadata = [ordered]@{
    schemaVersion = 1
    projectName = $ProjectName
    projectType = $ProjectType
    architecture = 'x86_64'
    bootMethod = 'Multiboot'
    osVersion = $OsVersion
    author = [ordered]@{ name = $AuthorName; email = $AuthorEmail }
    license = $License
} | ConvertTo-Json -Depth 4
$fileVersions = [ordered]@{
    schemaVersion = 1
    files = [ordered]@{
        'Include/OSVersion.h' = $FileVersion
        'Source/kmain.c' = $FileVersion
    }
} | ConvertTo-Json -Depth 4
$authors = "$ProjectName`r`n`r`nPrimary author: $AuthorName`r`nEmail: $AuthorEmail`r`n"

$sdkRoot = [Environment]::GetEnvironmentVariable('OESDK_ROOT', 'Machine')
if ([string]::IsNullOrWhiteSpace($sdkRoot)) { $sdkRoot = $env:OESDK_ROOT }
if ([string]::IsNullOrWhiteSpace($sdkRoot) -or -not (Test-Path -LiteralPath $sdkRoot -PathType Container)) {
    throw 'OESDK_ROOT is not set. Repair or reinstall OESDK.'
}
$licenseFile = switch ($License) {
    'LicenseRef-Proprietary' { 'Proprietary.txt' }
    'LicenseRef-Custom' { 'LicenseRef-Custom.txt' }
    default { $License + '.txt' }
}
$licensePath = Join-Path $sdkRoot (Join-Path 'Licenses\Project' $licenseFile)
if (-not (Test-Path -LiteralPath $licensePath -PathType Leaf)) { throw "The OESDK licence text is missing: $licenseFile" }
$licenseText = [IO.File]::ReadAllText($licensePath).Replace('__OESDK_YEAR__', [DateTime]::Now.Year.ToString()).Replace('__OESDK_AUTHOR_NAME__', $AuthorName)

$readme = @"
# $ProjectName

This is an OESDK 0.0.12 native Clang C $ProjectType project. It is a `.vcxproj`,
not a C# `.csproj`, and does not require the Microsoft .NET SDK.

OS version: $OsVersion
Author: $AuthorName <$AuthorEmail>
Licence: $License

Build Debug or Release in Visual Studio, then use Start Without Debugging to
launch `kernel.elf` in QEMU. Debug builds also send `kdebugf` output to COM1.
"@

$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText((Join-Path $projectRoot ($ProjectName + '.vcxproj')), $projectXml, $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'Source\kmain.c'), $source, $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'Include\OSVersion.h'), $versionHeader, $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'OESDKProject.json'), $projectMetadata + "`r`n", $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'OESDKFileVersions.json'), $fileVersions + "`r`n", $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'AUTHORS'), $authors, $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'LICENSE'), $licenseText, $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'README.md'), $readme, $utf8)

Write-Host "[ OK ] Created native OESDK $ProjectType project: $projectRoot"
Write-Host "[ OK ] Open: $(Join-Path $projectRoot ($ProjectName + '.vcxproj'))"
