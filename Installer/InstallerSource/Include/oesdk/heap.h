#ifndef OESDK_HEAP_H
#define OESDK_HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_HEAP_BLOCK_MAGIC UINT64_C(0x4F4553444B484541)
#define OESDK_HEAP_MINIMUM_BLOCK_SIZE 16U
#define OESDK_HEAP_DEFAULT_ALIGNMENT ((size_t)sizeof(uintptr_t))

typedef struct OesdkHeapBlock
{
    uint64_t Magic;
    size_t Size;
    bool Free;
    struct OesdkHeapBlock *Previous;
    struct OesdkHeapBlock *Next;
} OesdkHeapBlock;

typedef struct OesdkHeapInformation
{
    uintptr_t Base;
    uintptr_t Current;
    uintptr_t End;
    size_t Capacity;
    size_t Used;
    size_t Remaining;
    size_t AllocationCount;
    size_t ActiveAllocationCount;
    size_t FreeBlockCount;
    size_t LargestFreeBlock;
    bool Initialized;
    bool PermanentInitialized;
} OesdkHeapInformation;

OesdkStatus OesdkHeapBootstrapInitialize(void *Base, size_t Size);
void *OesdkHeapBootstrapAllocate(size_t Size, size_t Alignment);
void *OesdkHeapBootstrapAllocateZeroed(size_t Count, size_t Size, size_t Alignment);
bool OesdkHeapBootstrapIsInitialized(void);

OesdkStatus OesdkHeapInitialize(void *Base, size_t Size);
void *OesdkAllocate(size_t Size);
void *OesdkAllocateAligned(size_t Size, size_t Alignment);
void *OesdkAllocateZeroed(size_t Count, size_t Size);
void OesdkFree(void *Address);
bool OesdkHeapIsInitialized(void);

const OesdkHeapInformation *OesdkHeapInformationGet(void);

#ifdef __cplusplus
}
#endif

#endif
