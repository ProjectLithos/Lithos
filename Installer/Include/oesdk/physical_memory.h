#ifndef OESDK_PHYSICAL_MEMORY_H
#define OESDK_PHYSICAL_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/boot_context.h>
#include <oesdk/memory_map.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_PAGE_SIZE 4096ULL
#define OESDK_PAGE_SHIFT 12U
#define OESDK_PAGE_MASK (OESDK_PAGE_SIZE - 1ULL)
#define OESDK_PHYSICAL_MAX_PAGES 4194304ULL
#define OESDK_PHYSICAL_MAX_BYTES (OESDK_PHYSICAL_MAX_PAGES * OESDK_PAGE_SIZE)

#define OESDK_IS_POWER_OF_TWO(Value) \
    ((Value) != 0U && (((Value) & ((Value) - 1U)) == 0U))
#define OESDK_ALIGN_DOWN(Value, Alignment) \
    ((Value) & ~((Alignment) - 1U))
#define OESDK_ALIGN_UP(Value, Alignment) \
    (((Value) + ((Alignment) - 1U)) & ~((Alignment) - 1U))

typedef struct OesdkPhysicalMemoryInformation
{
    uint64_t PageSize;
    uint64_t PageCount;
    uint64_t BitmapBytes;
    uint64_t FreePageCount;
    uint64_t ReservedPageCount;
    uint64_t HighestPhysicalAddress;
} OesdkPhysicalMemoryInformation;

bool OesdkAlignmentIsPowerOfTwo(uint64_t Value);
bool OesdkAlignDownU64(uint64_t Value, uint64_t Alignment, uint64_t *Result);
bool OesdkAlignUpU64(uint64_t Value, uint64_t Alignment, uint64_t *Result);
bool OesdkPhysicalAddressToPage(uint64_t PhysicalAddress, uint64_t *PageNumber);
bool OesdkPhysicalPageToAddress(uint64_t PageNumber, uint64_t *PhysicalAddress);

OesdkStatus OesdkPhysicalMemoryInitialize(
    const OesdkMemoryRegion *Regions,
    size_t RegionCount,
    const OesdkBootContext *BootContext);
OesdkStatus OesdkPhysicalMemoryReserve(uint64_t Base, uint64_t Length, bool Permanent);
OesdkStatus OesdkPhysicalMemoryAllocate(size_t PageCount, uint64_t *PhysicalAddress);
OesdkStatus OesdkPhysicalMemoryFree(uint64_t PhysicalAddress, size_t PageCount);
bool OesdkPhysicalMemoryPageIsAllocated(uint64_t PageNumber);
bool OesdkPhysicalMemoryPageIsPermanent(uint64_t PageNumber);
bool OesdkPhysicalMemoryIsInitialized(void);
const OesdkPhysicalMemoryInformation *OesdkPhysicalMemoryInformationGet(void);

#ifdef __cplusplus
}
#endif

#endif
