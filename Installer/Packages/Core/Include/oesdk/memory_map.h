#ifndef OESDK_MEMORY_MAP_H
#define OESDK_MEMORY_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_MEMORY_MAP_MAX_REGIONS 256U

typedef enum OesdkMemoryType
{
    OesdkMemoryReserved = 0,
    OesdkMemoryAvailable,
    OesdkMemoryAcpiReclaimable,
    OesdkMemoryAcpiNvs,
    OesdkMemoryBad,
    OesdkMemoryKernel,
    OesdkMemoryBootData,
    OesdkMemoryFramebuffer
} OesdkMemoryType;

typedef struct OesdkMemoryRegion
{
    uint64_t Base;
    uint64_t Length;
    OesdkMemoryType Type;
} OesdkMemoryRegion;

bool OesdkMemoryRangeEnd(const OesdkMemoryRegion *Region, uint64_t *End);
bool OesdkMemoryRangeIsValid(const OesdkMemoryRegion *Region);
bool OesdkMemoryRangesOverlap(const OesdkMemoryRegion *Left, const OesdkMemoryRegion *Right);
OesdkMemoryType OesdkMemoryTypeFromMultiboot1(uint32_t BootloaderType);
OesdkStatus OesdkMemoryMapNormalizeMultiboot1(const void *MemoryMap, size_t MemoryMapBytes);
const OesdkMemoryRegion *OesdkMemoryMapRegions(void);
size_t OesdkMemoryMapRegionCount(void);

#ifdef __cplusplus
}
#endif

#endif
