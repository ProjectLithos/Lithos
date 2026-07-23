#include "Usage.h"

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

static OesdkStatus TemplateNormalizeBootMemoryMap(const OesdkBootContext *BootContext)
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
        Information->MemoryMapAddress == 0U || Information->MemoryMapLength == 0U) {
        return OESDK_STATUS_NOT_FOUND;
    }

    return OesdkMemoryMapNormalizeMultiboot1(
        (const void *)(uintptr_t)Information->MemoryMapAddress,
        (size_t)Information->MemoryMapLength);
}

void TemplateUsageBoot(const OesdkBootContext *BootContext)
{
    OesdkStatus Status;
    size_t Index;

    OESDK_ASSERT(BootContext != NULL);
    kprintf("OESDK version: %u.%u.%u.%u\n", OESDK_VERSION_MAJOR,
            OESDK_VERSION_MINOR, OESDK_VERSION_PATCH, OESDK_VERSION_REVISION);
    kprintf("Boot ABI=%u protocol=%u kernel=0x%llx-0x%llx\n",
            (unsigned int)BootContext->AbiVersion,
            (unsigned int)BootContext->BootProtocol,
            (unsigned long long)BootContext->KernelPhysicalStart,
            (unsigned long long)BootContext->KernelPhysicalEnd);

    Status = TemplateNormalizeBootMemoryMap(BootContext);
    kprintf("Memory-map normalisation: %s (%s)\n",
            OesdkStatusName(Status), OesdkStatusDescription(Status));
    if (OESDK_FAILED(Status)) return;

    for (Index = 0U; Index < OesdkMemoryMapRegionCount(); ++Index) {
        const OesdkMemoryRegion *Region = &OesdkMemoryMapRegions()[Index];
        uint64_t End = 0U;
        if (!OesdkMemoryRangeEnd(Region, &End)) {
            OESDK_PANIC("MemoryMap", "A normalised range became invalid", 0x18200002ULL);
        }
        kdebugf("Memory[%llu] 0x%llx-0x%llx type=%u\n",
                (unsigned long long)Index, (unsigned long long)Region->Base,
                (unsigned long long)End, (unsigned int)Region->Type);
    }
}
