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

#define OESDK_X86_64_PAGE_OFFSET_BITS 12U
#define OESDK_X86_64_PT_SHIFT 12U
#define OESDK_X86_64_PD_SHIFT 21U
#define OESDK_X86_64_PDPT_SHIFT 30U
#define OESDK_X86_64_PML4_SHIFT 39U
#define OESDK_X86_64_INDEX_MASK 0x1FFULL
#define OESDK_X86_64_OFFSET_MASK 0xFFFULL
#define OESDK_X86_64_CANONICAL_BIT 47U
#define OESDK_X86_64_LOWER_CANONICAL_MAX 0x00007FFFFFFFFFFFULL
#define OESDK_X86_64_UPPER_CANONICAL_MIN 0xFFFF800000000000ULL
#define OESDK_KERNEL_HIGHER_HALF_BASE 0xFFFFFFFF80000000ULL

#define OESDK_VIRTUAL_FLAG_PRESENT       (1ULL << 0)
#define OESDK_VIRTUAL_FLAG_WRITABLE      (1ULL << 1)
#define OESDK_VIRTUAL_FLAG_USER          (1ULL << 2)
#define OESDK_VIRTUAL_FLAG_WRITE_THROUGH (1ULL << 3)
#define OESDK_VIRTUAL_FLAG_CACHE_DISABLE (1ULL << 4)
#define OESDK_VIRTUAL_FLAG_GLOBAL        (1ULL << 8)
#define OESDK_VIRTUAL_FLAG_NO_EXECUTE    (1ULL << 63)
#define OESDK_VIRTUAL_FLAGS_ALL          (OESDK_VIRTUAL_FLAG_PRESENT | OESDK_VIRTUAL_FLAG_WRITABLE | \
                                          OESDK_VIRTUAL_FLAG_USER | OESDK_VIRTUAL_FLAG_WRITE_THROUGH | \
                                          OESDK_VIRTUAL_FLAG_CACHE_DISABLE | OESDK_VIRTUAL_FLAG_GLOBAL | \
                                          OESDK_VIRTUAL_FLAG_NO_EXECUTE)

typedef uint64_t OesdkVirtualFlags;

typedef struct OesdkVirtualAddressParts
{
    uint16_t Pml4Index;
    uint16_t PdptIndex;
    uint16_t PdIndex;
    uint16_t PtIndex;
    uint16_t Offset;
    bool UpperHalf;
    bool Canonical;
} OesdkVirtualAddressParts;

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
void OesdkVirtualAddressDecode(uintptr_t VirtualAddress, OesdkVirtualAddressParts *Parts);
OesdkStatus OesdkVirtualMemoryInitialize(void);
OesdkStatus OesdkVirtualMap(
    uintptr_t VirtualAddress,
    uintptr_t PhysicalAddress,
    size_t PageCount,
    uint64_t Flags);
OesdkStatus OesdkVirtualUnmap(uintptr_t VirtualAddress, size_t PageCount);
bool OesdkVirtualTranslate(uintptr_t VirtualAddress, uintptr_t *PhysicalAddress);
bool OesdkVirtualMemoryIsInitialized(void);
const OesdkVirtualMemoryInformation *OesdkVirtualMemoryInformationGet(void);

#ifdef __cplusplus
}
#endif

#endif
