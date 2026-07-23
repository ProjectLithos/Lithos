#include <oesdk/heap.h>

static OesdkHeapInformation OesdkHeapState;

static bool AddOverflow(uintptr_t A, uintptr_t B, uintptr_t *Result)
{
    uintptr_t Value = A + B;
    if (Value < A) return true;
    *Result = Value;
    return false;
}

static bool IsPowerOfTwo(size_t Value)
{
    return Value != 0U && (Value & (Value - 1U)) == 0U;
}

OesdkStatus OesdkHeapBootstrapInitialize(void *Base, size_t Size)
{
    uintptr_t Start;
    uintptr_t End;
    if (Base == NULL || Size == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    Start = (uintptr_t)Base;
    if (AddOverflow(Start, (uintptr_t)Size, &End)) return OESDK_STATUS_INVALID_ARGUMENT;
    OesdkHeapState.Base = Start;
    OesdkHeapState.Current = Start;
    OesdkHeapState.End = End;
    OesdkHeapState.Capacity = Size;
    OesdkHeapState.Used = 0U;
    OesdkHeapState.Remaining = Size;
    OesdkHeapState.AllocationCount = 0U;
    OesdkHeapState.Initialized = true;
    return OESDK_STATUS_SUCCESS;
}

void *OesdkHeapBootstrapAllocate(size_t Size, size_t Alignment)
{
    uintptr_t Mask;
    uintptr_t Result;
    uintptr_t Next;
    if (!OesdkHeapState.Initialized || Size == 0U || !IsPowerOfTwo(Alignment)) return NULL;
    Mask = (uintptr_t)Alignment - 1U;
    if (OesdkHeapState.Current > UINTPTR_MAX - Mask) return NULL;
    Result = (OesdkHeapState.Current + Mask) & ~Mask;
    if (AddOverflow(Result, (uintptr_t)Size, &Next) || Next > OesdkHeapState.End) return NULL;
    OesdkHeapState.Current = Next;
    OesdkHeapState.Used = (size_t)(Next - OesdkHeapState.Base);
    OesdkHeapState.Remaining = (size_t)(OesdkHeapState.End - Next);
    OesdkHeapState.AllocationCount++;
    return (void *)Result;
}

void *OesdkHeapBootstrapAllocateZeroed(size_t Count, size_t Size, size_t Alignment)
{
    size_t Total;
    unsigned char *Memory;
    size_t Index;
    if (Count != 0U && Size > ((size_t)-1) / Count) return NULL;
    Total = Count * Size;
    Memory = (unsigned char *)OesdkHeapBootstrapAllocate(Total, Alignment);
    if (Memory == NULL) return NULL;
    for (Index = 0U; Index < Total; ++Index) Memory[Index] = 0U;
    return Memory;
}

bool OesdkHeapBootstrapIsInitialized(void)
{
    return OesdkHeapState.Initialized;
}

const OesdkHeapInformation *OesdkHeapInformationGet(void)
{
    return &OesdkHeapState;
}
