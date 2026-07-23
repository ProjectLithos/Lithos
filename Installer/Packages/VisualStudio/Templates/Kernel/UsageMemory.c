#include "Usage.h"

static unsigned char TemplateBootstrapHeap[256U * 1024U];
static unsigned char TemplatePermanentHeap[1024U * 1024U];

void TemplateUsageMemory(const OesdkBootContext *BootContext)
{
    OesdkStatus Status;
    uint64_t PhysicalAddress = 0U;
    void *Allocation;
    void *Zeroed;
    const OesdkPhysicalMemoryInformation *Physical;
    const OesdkHeapInformation *Heap;

    if (OesdkMemoryMapRegionCount() != 0U) {
        Status = OesdkPhysicalMemoryInitialize(OesdkMemoryMapRegions(),
                                               OesdkMemoryMapRegionCount(), BootContext);
        kprintf("Physical allocator: %s\n", OesdkStatusName(Status));
        if (OESDK_SUCCEEDED(Status)) {
            Status = OesdkVirtualMemoryInitialize();
            OESDK_ASSERT(OESDK_SUCCEEDED(Status));
            Physical = OesdkPhysicalMemoryInformationGet();
            OESDK_ASSERT(Physical != NULL);
            kprintf("Physical pages: total=%llu free=%llu\n",
                    (unsigned long long)Physical->PageCount,
                    (unsigned long long)Physical->FreePageCount);
            Status = OesdkPhysicalMemoryAllocate(1U, &PhysicalAddress);
            if (OESDK_SUCCEEDED(Status)) {
                OESDK_ASSERT(OESDK_SUCCEEDED(OesdkPhysicalMemoryFree(PhysicalAddress, 1U)));
            }
        }
    }

    Status = OesdkHeapBootstrapInitialize(TemplateBootstrapHeap, sizeof(TemplateBootstrapHeap));
    OESDK_ASSERT(OESDK_SUCCEEDED(Status));
    OESDK_ASSERT(OesdkHeapBootstrapAllocate(96U, 16U) != NULL);
    OESDK_ASSERT(OesdkHeapBootstrapAllocateZeroed(32U, sizeof(uint32_t), 16U) != NULL);

    Status = OesdkHeapInitialize(TemplatePermanentHeap, sizeof(TemplatePermanentHeap));
    OESDK_ASSERT(OESDK_SUCCEEDED(Status));
    Allocation = OesdkAllocate(96U);
    Zeroed = OesdkAllocateZeroed(32U, sizeof(uint32_t));
    OESDK_ASSERT(Allocation != NULL && Zeroed != NULL);
    OesdkFree(Allocation);
    OesdkFree(Zeroed);
    Heap = OesdkHeapInformationGet();
    kprintf("Heap: remaining=%llu largest=%llu active=%llu\n",
            (unsigned long long)Heap->Remaining,
            (unsigned long long)Heap->LargestFreeBlock,
            (unsigned long long)Heap->ActiveAllocationCount);
}
