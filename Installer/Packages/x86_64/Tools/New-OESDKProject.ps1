[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ProjectName,

    [ValidateSet('Kernel', 'Desktop')]
    [string]$ProjectType = 'Kernel',

    [string]$Destination = (Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'OESDK Projects')
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ($ProjectName -notmatch '^[A-Za-z_][A-Za-z0-9_.-]*$') {
    throw 'ProjectName must begin with a letter or underscore and contain only letters, numbers, dots, underscores, or hyphens.'
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
    <NMakeBuildCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Build-Kernel.ps1&quot; -ProjectRoot &quot;$(ProjectDir)&quot; -Configuration &quot;$(Configuration)&quot; -ProjectType __PROJECT_TYPE__</NMakeBuildCommandLine>
    <NMakeReBuildCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Build-Kernel.ps1&quot; -ProjectRoot &quot;$(ProjectDir)&quot; -Configuration &quot;$(Configuration)&quot; -ProjectType __PROJECT_TYPE__ -Clean; powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Build-Kernel.ps1&quot; -ProjectRoot &quot;$(ProjectDir)&quot; -Configuration &quot;$(Configuration)&quot; -ProjectType __PROJECT_TYPE__</NMakeReBuildCommandLine>
    <NMakeCleanCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Build-Kernel.ps1&quot; -ProjectRoot &quot;$(ProjectDir)&quot; -Configuration &quot;$(Configuration)&quot; -ProjectType __PROJECT_TYPE__ -Clean</NMakeCleanCommandLine>
    <NMakeOutput>$(ProjectDir)Build\$(Configuration)\kernel.elf</NMakeOutput>
    <NMakeIncludeSearchPath>$(OESDK_ROOT)\Include;$(ProjectDir)Include</NMakeIncludeSearchPath>
    <LocalDebuggerCommand>powershell.exe</LocalDebuggerCommand>
    <LocalDebuggerCommandArguments>-NoProfile -ExecutionPolicy Bypass -File &quot;$(OESDK_ROOT)\Tools\Run-Qemu.ps1&quot; -Kernel &quot;$(ProjectDir)Build\$(Configuration)\kernel.elf&quot;</LocalDebuggerCommandArguments>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <ItemGroup><ClCompile Include="Source\kmain.c" /></ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
'@
$projectXml = $projectXml.Replace('__PROJECT_GUID__', $projectGuid).Replace('__PROJECT_NAME__', $escapedName).Replace('__PROJECT_TYPE__', $ProjectType)

if ($ProjectType -eq 'Desktop') {
    $source = @'
#include <oesdk/kernel.h>

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

void kmain(int argc, char *argv)
{
    (void)argc;
    (void)argv;

    kdebugf("__PROJECT_NAME__: Hello from the serial debug port!\n");
    kprintf("__PROJECT_NAME__: Hello Screen!\n");
}
'@
}
$source = $source.Replace('__PROJECT_NAME__', $ProjectName)

$readme = @"
# $ProjectName

This is an OESDK 0.0.7 native Clang C $ProjectType project. It is a `.vcxproj`,
not a C# `.csproj`, and does not require the Microsoft .NET SDK.

Build Debug or Release in Visual Studio, then use Start Without Debugging to
launch `kernel.elf` in QEMU. Debug builds also send `kdebugf` output to COM1.
"@

$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText((Join-Path $projectRoot ($ProjectName + '.vcxproj')), $projectXml, $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'Source\kmain.c'), $source, $utf8)
[IO.File]::WriteAllText((Join-Path $projectRoot 'README.md'), $readme, $utf8)

Write-Host "[ OK ] Created native OESDK $ProjectType project: $projectRoot"
Write-Host "[ OK ] Open: $(Join-Path $projectRoot ($ProjectName + '.vcxproj'))"
