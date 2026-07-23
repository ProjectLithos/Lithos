#ifndef OESDK_HEAP_H
#define OESDK_HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OesdkHeapInformation
{
    uintptr_t Base;
    uintptr_t Current;
    uintptr_t End;
    size_t Capacity;
    size_t Used;
    size_t Remaining;
    size_t AllocationCount;
    bool Initialized;
} OesdkHeapInformation;

OesdkStatus OesdkHeapBootstrapInitialize(void *Base, size_t Size);
void *OesdkHeapBootstrapAllocate(size_t Size, size_t Alignment);
void *OesdkHeapBootstrapAllocateZeroed(size_t Count, size_t Size, size_t Alignment);
bool OesdkHeapBootstrapIsInitialized(void);
const OesdkHeapInformation *OesdkHeapInformationGet(void);

#ifdef __cplusplus
}
#endif

#endif
