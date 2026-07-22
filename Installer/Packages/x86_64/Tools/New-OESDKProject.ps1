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
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
'@
$projectDefinition = if ($ProjectType -eq 'Desktop') { ';OESDK_DESKTOP=1' } else { '' }
$projectXml = $projectXml.Replace('__PROJECT_GUID__', $projectGuid).Replace('__PROJECT_NAME__', $escapedName).Replace('__PROJECT_TYPE__', $ProjectType).Replace('__PROJECT_DEFINITION__', $projectDefinition)

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

typedef struct __attribute__((packed)) TemplateMultibootInformation
{
    uint32_t Flags;
    uint32_t MemoryLowerKilobytes;
    uint32_t MemoryUpperKilobytes;
    uint32_t BootDevice;
    uint32_t CommandLine;
    uint32_t ModulesCount;
    uint32_t ModulesAddress;
    uint32_t Symbols[4];
    uint32_t MemoryMapLength;
    uint32_t MemoryMapAddress;
} TemplateMultibootInformation;

static void TemplateInterruptHandler(OesdkInterruptFrame *Frame, void *Context)
{
    (void)Context;
    if (Frame != NULL) {
        kdebugf("__PROJECT_NAME__: interrupt vector %u received.\n",
                (unsigned int)Frame->Vector);
    }
}

static void PrintBootContext(const OesdkBootContext *BootContext)
{
    OESDK_ASSERT(BootContext != NULL);

    kprintf("OESDK version: %u.%u.%u.%u\n",
            OESDK_VERSION_MAJOR,
            OESDK_VERSION_MINOR,
            OESDK_VERSION_PATCH,
            OESDK_VERSION_REVISION);
    kprintf("Boot ABI: %u, protocol: %u\n",
            (unsigned int)BootContext->AbiVersion,
            (unsigned int)BootContext->BootProtocol);
    kprintf("Kernel: 0x%llx-0x%llx, stack: 0x%llx-0x%llx\n",
            (unsigned long long)BootContext->KernelPhysicalStart,
            (unsigned long long)BootContext->KernelPhysicalEnd,
            (unsigned long long)BootContext->BootstrapStackStart,
            (unsigned long long)BootContext->BootstrapStackEnd);
    if (BootContext->MemorySizeAvailable) {
        kprintf("Available memory: %llu bytes\n",
                (unsigned long long)BootContext->AvailableMemoryBytes);
    }
}

static void PrintCpuInformation(void)
{
    const OesdkCpuFeatures *Features = OesdkCpuFeaturesGet();
    OesdkCpuRegisters Registers;

    OESDK_ASSERT(Features != NULL);
    OesdkCpuQuery(0U, 0U, &Registers);
    kprintf("CPU vendor: %s, CPUID max leaf: 0x%x\n",
            Features->Vendor,
            (unsigned int)Registers.Eax);
    kprintf("CPU: long-mode=%u NX=%u APIC=%u x2APIC=%u syscall=%u TSC=%u\n",
            Features->LongMode ? 1U : 0U,
            Features->Nx ? 1U : 0U,
            Features->Apic ? 1U : 0U,
            Features->X2Apic ? 1U : 0U,
            Features->Syscall ? 1U : 0U,
            Features->Tsc ? 1U : 0U);
}

static void PrintDescriptorInformation(void)
{
    const OesdkGdtInformation *Gdt = OesdkGdtInformationGet();
    const OesdkIdtInformation *Idt = OesdkIdtInformationGet();

    OESDK_ASSERT(OesdkGdtIsInitialized());
    OESDK_ASSERT(Gdt != NULL && Gdt->Initialized);
    OESDK_ASSERT(Idt != NULL && Idt->Initialized);

    kprintf("GDT: %u descriptors, TSS selector 0x%x, IST entries %u\n",
            (unsigned int)Gdt->DescriptorCount,
            (unsigned int)Gdt->TaskStateSelector,
            OESDK_GDT_IST_ENTRY_COUNT);
    kprintf("IDT: %u vectors at 0x%llx\n",
            (unsigned int)Idt->InstalledVectors,
            (unsigned long long)Idt->Base);
    kprintf("Exception 14: %s\n", OesdkExceptionName(14U));
}

static OesdkStatus NormalizeBootMemoryMap(const OesdkBootContext *BootContext)
{
    const uint32_t MemoryMapFlag = (1U << 6U);
    const TemplateMultibootInformation *Information;

    if (BootContext == NULL ||
        BootContext->BootProtocol != OesdkBootProtocolMultiboot1 ||
        BootContext->BootInformationAddress == 0U) {
        return OESDK_STATUS_INVALID_ARGUMENT;
    }

    Information = (const TemplateMultibootInformation *)BootContext->BootInformationAddress;
    if ((Information->Flags & MemoryMapFlag) == 0U ||
        Information->MemoryMapAddress == 0U ||
        Information->MemoryMapLength == 0U) {
        return OESDK_STATUS_NOT_FOUND;
    }

    return OesdkMemoryMapNormalizeMultiboot1(
        (const void *)(uintptr_t)Information->MemoryMapAddress,
        (size_t)Information->MemoryMapLength);
}

static void PrintMemoryMap(const OesdkBootContext *BootContext)
{
    OesdkStatus Status = NormalizeBootMemoryMap(BootContext);
    size_t Index;

    kprintf("Memory-map normalisation: %s (%s)\n",
            OesdkStatusName(Status),
            OesdkStatusDescription(Status));
    if (OESDK_FAILED(Status)) {
        return;
    }

    kprintf("Normalised regions: %llu\n",
            (unsigned long long)OesdkMemoryMapRegionCount());
    for (Index = 0U; Index < OesdkMemoryMapRegionCount(); ++Index) {
        const OesdkMemoryRegion *Region = &OesdkMemoryMapRegions()[Index];
        uint64_t End = 0U;
        if (!OesdkMemoryRangeEnd(Region, &End)) {
            OESDK_PANIC("MemoryMap", "A normalised range became invalid", 0x17160001ULL);
        }
        kdebugf("Memory[%llu] 0x%llx-0x%llx type=%u\n",
                (unsigned long long)Index,
                (unsigned long long)Region->Base,
                (unsigned long long)End,
                (unsigned int)Region->Type);
    }
}

static void DemonstrateInterruptApi(void)
{
    OesdkStatus Status = OesdkInterruptHandlerRegister(
        OESDK_INTERRUPT_VECTOR_FIRST,
        TemplateInterruptHandler,
        NULL);

    kprintf("Interrupt registration: %s\n", OesdkStatusName(Status));
    if (OESDK_SUCCEEDED(Status)) {
        OESDK_ASSERT(OesdkInterruptHandlerIsRegistered(OESDK_INTERRUPT_VECTOR_FIRST));
        Status = OesdkInterruptHandlerUnregister(
            OESDK_INTERRUPT_VECTOR_FIRST,
            TemplateInterruptHandler);
        OESDK_ASSERT(OESDK_SUCCEEDED(Status));
    }
}

static void DemonstrateGraphics(void)
{
    if (!kgfx_available()) {
        kprintf("Graphics: no framebuffer surface supplied.\n");
        return;
    }

    const kgfx_surface *Screen = kgfx_screen();
    OESDK_ASSERT(Screen != NULL);
    kgfx_rectangle(8U, 8U,
                   Screen->width > 16U ? Screen->width - 16U : 0U,
                   4U,
                   0x40C080U);
    kgfx_pixel(8U, 16U, 0xFFFFFFU);
    kprintf("Graphics: %ux%u, pitch=%u.\n",
            Screen->width, Screen->height, Screen->pitch);
}

void kmain(int argc, char *argv)
{
    const OesdkBootContext *BootContext;
    uint64_t SavedInterruptState;

    (void)argc;
    (void)argv;

    kclear();
    kputs("__PROJECT_NAME__: OESDK base template\n");
    kdebugf("__PROJECT_NAME__: serial debugging is active.\n");

    BootContext = OesdkBootContextGet();
    OESDK_ASSERT_CODE(BootContext != NULL, 0x17160002ULL);

    PrintBootContext(BootContext);
    PrintCpuInformation();
    PrintDescriptorInformation();
    PrintMemoryMap(BootContext);
    DemonstrateInterruptApi();
    DemonstrateGraphics();

    SavedInterruptState = OesdkCpuInterruptStateSave();
    OesdkCpuPause();
    OesdkCpuInterruptStateRestore(SavedInterruptState);

    kprintf("__PROJECT_NAME__: all current core SDK services are available.\n");
}
'@
}
$source = $source.Replace('__PROJECT_NAME__', $ProjectName)

$readme = @"
# $ProjectName

This is an OESDK 0.17.16 native Clang C $ProjectType project. It is a `.vcxproj`,
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
