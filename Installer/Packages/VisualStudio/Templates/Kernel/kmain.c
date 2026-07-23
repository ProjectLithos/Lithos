#include <oesdk/kernel.h>

static unsigned char TemplateBootstrapHeap[256U * 1024U] __attribute__((aligned(4096)));
static unsigned char TemplatePermanentHeap[1024U * 1024U] __attribute__((aligned(4096)));

typedef struct TemplateMultibootInformation
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
        kdebugf("$safeprojectname$: interrupt vector %u received.\n",
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
            OESDK_PANIC("MemoryMap", "A normalised range became invalid", 0x17170001ULL);
        }
        kdebugf("Memory[%llu] 0x%llx-0x%llx type=%u\n",
                (unsigned long long)Index,
                (unsigned long long)Region->Base,
                (unsigned long long)End,
                (unsigned int)Region->Type);
    }
}

static void DemonstratePhysicalMemory(const OesdkBootContext *BootContext)
{
    OesdkStatus Status;
    uint64_t PhysicalAddress = 0U;
    const OesdkPhysicalMemoryInformation *Memory;

    if (OesdkMemoryMapRegionCount() == 0U) {
        kprintf("Physical allocator: no normalised memory map.\n");
        return;
    }

    Status = OesdkPhysicalMemoryInitialize(
        OesdkMemoryMapRegions(),
        OesdkMemoryMapRegionCount(),
        BootContext);
    kprintf("Physical allocator: %s\n", OesdkStatusName(Status));
    if (OESDK_FAILED(Status)) return;

    Status = OesdkVirtualMemoryInitialize();
    kprintf("Virtual memory: %s\n", OesdkStatusName(Status));
    OESDK_ASSERT(OESDK_SUCCEEDED(Status));

    Memory = OesdkPhysicalMemoryInformationGet();
    OESDK_ASSERT(Memory != NULL);
    kprintf("Physical pages: total=%llu free=%llu reserved=%llu bitmap=%llu bytes\n",
            (unsigned long long)Memory->PageCount,
            (unsigned long long)Memory->FreePageCount,
            (unsigned long long)Memory->ReservedPageCount,
            (unsigned long long)Memory->BitmapBytes);

    Status = OesdkPhysicalMemoryAllocate(1U, &PhysicalAddress);
    if (OESDK_SUCCEEDED(Status)) {
        kprintf("Allocated test frame: 0x%llx\n", (unsigned long long)PhysicalAddress);
        OESDK_ASSERT(OESDK_SUCCEEDED(OesdkPhysicalMemoryFree(PhysicalAddress, 1U)));
    }
}


static void DemonstrateKernelHeap(void)
{
    OesdkStatus Status = OesdkHeapBootstrapInitialize(TemplateBootstrapHeap, sizeof(TemplateBootstrapHeap));
    void *First;
    void *Zeroed;
    const OesdkHeapInformation *Information;
    kprintf("Bootstrap heap: %s\n", OesdkStatusName(Status));
    OESDK_ASSERT(OESDK_SUCCEEDED(Status));
    First = OesdkHeapBootstrapAllocate(96U, 16U);
    Zeroed = OesdkHeapBootstrapAllocateZeroed(32U, sizeof(uint32_t), 16U);
    OESDK_ASSERT(First != NULL && Zeroed != NULL);
    Information = OesdkHeapInformationGet();
    kprintf("Bootstrap heap: capacity=%llu used=%llu remaining=%llu allocations=%llu\n",
            (unsigned long long)Information->Capacity,
            (unsigned long long)Information->Used,
            (unsigned long long)Information->Remaining,
            (unsigned long long)Information->AllocationCount);

    Status = OesdkHeapInitialize(TemplatePermanentHeap, sizeof(TemplatePermanentHeap));
    kprintf("Permanent heap: %s\n", OesdkStatusName(Status));
    OESDK_ASSERT(OESDK_SUCCEEDED(Status));
    First = OesdkAllocate(96U);
    Zeroed = OesdkAllocateZeroed(32U, sizeof(uint32_t));
    OESDK_ASSERT(First != NULL && Zeroed != NULL);
    OesdkFree(First);
    OesdkFree(Zeroed);
    Information = OesdkHeapInformationGet();
    kprintf("Free-list heap: free=%llu blocks=%llu largest=%llu active=%llu\n",
            (unsigned long long)Information->Remaining,
            (unsigned long long)Information->FreeBlockCount,
            (unsigned long long)Information->LargestFreeBlock,
            (unsigned long long)Information->ActiveAllocationCount);
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

static void DemonstrateTimerMath(void)
{
    const uint32_t Hz = 100U;
    const uint64_t Ticks = 250U;
    kprintf("Timer conversion: %llu ticks at %u Hz = %llu ms, %llu ns\n",
            (unsigned long long)Ticks,
            (unsigned int)Hz,
            (unsigned long long)OesdkTimerTicksToMilliseconds(Ticks, Hz),
            (unsigned long long)OesdkTimerTicksToNanoseconds(Ticks, Hz));
    kprintf("Timer backends: PIT, Local APIC, x2APIC, HPET. Routing is controller-owned.\n");
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
    kputs("$safeprojectname$: OESDK base template\n");
    kdebugf("$safeprojectname$: serial debugging is active.\n");

    BootContext = OesdkBootContextGet();
    OESDK_ASSERT_CODE(BootContext != NULL, 0x17170002ULL);

    PrintBootContext(BootContext);
    PrintCpuInformation();
    PrintDescriptorInformation();
    PrintMemoryMap(BootContext);
    DemonstratePhysicalMemory(BootContext);
    DemonstrateKernelHeap();
    DemonstrateInterruptApi();
    DemonstrateTimerMath();
    DemonstrateGraphics();

    SavedInterruptState = OesdkCpuInterruptStateSave();
    OesdkCpuPause();
    OesdkCpuInterruptStateRestore(SavedInterruptState);

    kprintf("$safeprojectname$: all current core SDK services are available.\n");
}
