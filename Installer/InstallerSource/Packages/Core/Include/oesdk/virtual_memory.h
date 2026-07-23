#ifndef OESDK_VIRTUAL_MEMORY_H
#define OESDK_VIRTUAL_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_VIRTUAL_PAGE_SIZE 4096ULL
#define OESDK_VIRTUAL_PAGE_MASK (OESDK_VIRTUAL_PAGE_SIZE - 1ULL)
#define OESDK_VIRTUAL_ADDRESS_MASK 0x000FFFFFFFFFF000ULL

#define OESDK_VIRTUAL_FLAG_PRESENT       (1ULL << 0)
#define OESDK_VIRTUAL_FLAG_WRITABLE      (1ULL << 1)
#define OESDK_VIRTUAL_FLAG_USER          (1ULL << 2)
#define OESDK_VIRTUAL_FLAG_WRITE_THROUGH (1ULL << 3)
#define OESDK_VIRTUAL_FLAG_CACHE_DISABLE (1ULL << 4)
#define OESDK_VIRTUAL_FLAG_GLOBAL        (1ULL << 8)
#define OESDK_VIRTUAL_FLAG_NO_EXECUTE    (1ULL << 63)

typedef uint64_t OesdkVirtualFlags;

typedef struct OesdkVirtualMemoryInformation
{
    uint64_t RootPhysicalAddress;
    uint64_t MappedPageCount;
    uint64_t UnmappedPageCount;
    uint64_t AllocatedTableCount;
    bool NoExecuteEnabled;
    bool Initialized;
} OesdkVirtualMemoryInformation;

bool OesdkVirtualAddressIsCanonical(uintptr_t VirtualAddress);
OesdkStatus OesdkVirtualMemoryInitialize(void);
OesdkStatus OesdkVirtualMap(
    uintptr_t VirtualAddress,
    uintptr_t PhysicalAddress,
    size_t PageCount,
    OesdkVirtualFlags Flags);
OesdkStatus OesdkVirtualUnmap(uintptr_t VirtualAddress, size_t PageCount);
bool OesdkVirtualTranslate(uintptr_t VirtualAddress, uintptr_t *PhysicalAddress);
bool OesdkVirtualMemoryIsInitialized(void);
const OesdkVirtualMemoryInformation *OesdkVirtualMemoryInformationGet(void);

#ifdef __cplusplus
}
#endif

#endif
