#include <oesdk/heap.h>

static OesdkHeapInformation OesdkHeapState;
static OesdkHeapBlock *OesdkHeapFirstBlock;

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

static bool AlignUp(uintptr_t Value, size_t Alignment, uintptr_t *Result)
{
    uintptr_t Mask;
    if (Result == NULL || !IsPowerOfTwo(Alignment)) return false;
    Mask = (uintptr_t)Alignment - 1U;
    if (Value > UINTPTR_MAX - Mask) return false;
    *Result = (Value + Mask) & ~Mask;
    return true;
}

static uintptr_t BlockAddress(const OesdkHeapBlock *Block)
{
    return (uintptr_t)Block;
}

static uintptr_t BlockDataAddress(const OesdkHeapBlock *Block)
{
    return BlockAddress(Block) + (uintptr_t)sizeof(OesdkHeapBlock);
}

static uintptr_t BlockEndAddress(const OesdkHeapBlock *Block)
{
    return BlockDataAddress(Block) + (uintptr_t)Block->Size;
}

static bool BlockValid(const OesdkHeapBlock *Block)
{
    return Block != NULL && Block->Magic == OESDK_HEAP_BLOCK_MAGIC;
}

static void RefreshPermanentStatistics(void)
{
    OesdkHeapBlock *Block = OesdkHeapFirstBlock;
    size_t FreeBytes = 0U;
    size_t FreeBlocks = 0U;
    size_t Largest = 0U;
    size_t Active = 0U;
    while (BlockValid(Block)) {
        if (Block->Free) {
            FreeBytes += Block->Size;
            FreeBlocks++;
            if (Block->Size > Largest) Largest = Block->Size;
        } else {
            Active++;
        }
        Block = Block->Next;
    }
    OesdkHeapState.Remaining = FreeBytes;
    OesdkHeapState.Used = OesdkHeapState.Capacity >= FreeBytes ? OesdkHeapState.Capacity - FreeBytes : 0U;
    OesdkHeapState.ActiveAllocationCount = Active;
    OesdkHeapState.FreeBlockCount = FreeBlocks;
    OesdkHeapState.LargestFreeBlock = Largest;
}

static bool BlocksAreContiguous(const OesdkHeapBlock *Block, const OesdkHeapBlock *Next)
{
    return BlockValid(Block) && BlockValid(Next) && BlockEndAddress(Block) == BlockAddress(Next);
}

static void MergeWithNext(OesdkHeapBlock *Block)
{
    OesdkHeapBlock *Next;
    if (!BlockValid(Block) || !Block->Free) return;
    Next = Block->Next;
    if (!BlockValid(Next) || !Next->Free || !BlocksAreContiguous(Block, Next)) return;
    Block->Size += sizeof(OesdkHeapBlock) + Next->Size;
    Block->Next = Next->Next;
    if (Block->Next != NULL) Block->Next->Previous = Block;
    Next->Magic = 0U;
    Next->Size = 0U;
    Next->Free = false;
    Next->Previous = NULL;
    Next->Next = NULL;
}

static OesdkHeapBlock *CreateAllocatedBlock(OesdkHeapBlock *Original, uintptr_t HeaderAddress, size_t RequestedSize)
{
    uintptr_t OriginalAddress = BlockAddress(Original);
    uintptr_t OriginalEnd = BlockEndAddress(Original);
    size_t PrefixTotal = (size_t)(HeaderAddress - OriginalAddress);
    OesdkHeapBlock *Allocated;
    OesdkHeapBlock *OldPrevious = Original->Previous;
    OesdkHeapBlock *OldNext = Original->Next;
    uintptr_t AllocatedEnd;
    size_t SuffixTotal;

    if (PrefixTotal >= sizeof(OesdkHeapBlock) + OESDK_HEAP_MINIMUM_BLOCK_SIZE) {
        Original->Size = PrefixTotal - sizeof(OesdkHeapBlock);
        Original->Free = true;
        Allocated = (OesdkHeapBlock *)HeaderAddress;
        Allocated->Previous = Original;
        Original->Next = Allocated;
    } else {
        Allocated = Original;
        HeaderAddress = OriginalAddress;
        PrefixTotal = 0U;
        Allocated->Previous = OldPrevious;
    }

    Allocated->Magic = OESDK_HEAP_BLOCK_MAGIC;
    Allocated->Size = RequestedSize;
    Allocated->Free = false;
    Allocated->Next = OldNext;
    if (OldNext != NULL) OldNext->Previous = Allocated;

    AllocatedEnd = HeaderAddress + sizeof(OesdkHeapBlock) + RequestedSize;
    SuffixTotal = (size_t)(OriginalEnd - AllocatedEnd);
    if (SuffixTotal >= sizeof(OesdkHeapBlock) + OESDK_HEAP_MINIMUM_BLOCK_SIZE) {
        OesdkHeapBlock *Suffix = (OesdkHeapBlock *)AllocatedEnd;
        Suffix->Magic = OESDK_HEAP_BLOCK_MAGIC;
        Suffix->Size = SuffixTotal - sizeof(OesdkHeapBlock);
        Suffix->Free = true;
        Suffix->Previous = Allocated;
        Suffix->Next = OldNext;
        if (OldNext != NULL) OldNext->Previous = Suffix;
        Allocated->Next = Suffix;
    } else {
        Allocated->Size += SuffixTotal;
    }
    return Allocated;
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
    OesdkHeapState.ActiveAllocationCount = 0U;
    OesdkHeapState.FreeBlockCount = 0U;
    OesdkHeapState.LargestFreeBlock = 0U;
    OesdkHeapState.Initialized = true;
    OesdkHeapState.PermanentInitialized = false;
    OesdkHeapFirstBlock = NULL;
    return OESDK_STATUS_SUCCESS;
}

void *OesdkHeapBootstrapAllocate(size_t Size, size_t Alignment)
{
    uintptr_t Result;
    uintptr_t Next;
    if (!OesdkHeapState.Initialized || OesdkHeapState.PermanentInitialized || Size == 0U) return NULL;
    if (!AlignUp(OesdkHeapState.Current, Alignment, &Result)) return NULL;
    /* Result = AlignUp(Current, Alignment); Next = Result + Size. */
    if (AddOverflow(Result, (uintptr_t)Size, &Next)) return NULL;
    if (Next < Result || Next > OesdkHeapState.End) return NULL;
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
    return OesdkHeapState.Initialized && !OesdkHeapState.PermanentInitialized;
}

OesdkStatus OesdkHeapInitialize(void *Base, size_t Size)
{
    uintptr_t Start;
    uintptr_t End;
    uintptr_t AlignedStart;
    size_t Available;
    if (Base == NULL || Size <= sizeof(OesdkHeapBlock) + OESDK_HEAP_MINIMUM_BLOCK_SIZE) return OESDK_STATUS_INVALID_ARGUMENT;
    Start = (uintptr_t)Base;
    if (AddOverflow(Start, (uintptr_t)Size, &End)) return OESDK_STATUS_INVALID_ARGUMENT;
    if (!AlignUp(Start, OESDK_HEAP_DEFAULT_ALIGNMENT, &AlignedStart) || AlignedStart >= End) return OESDK_STATUS_INVALID_ARGUMENT;
    Available = (size_t)(End - AlignedStart);
    if (Available <= sizeof(OesdkHeapBlock) + OESDK_HEAP_MINIMUM_BLOCK_SIZE) return OESDK_STATUS_OUT_OF_MEMORY;

    OesdkHeapFirstBlock = (OesdkHeapBlock *)AlignedStart;
    OesdkHeapFirstBlock->Magic = OESDK_HEAP_BLOCK_MAGIC;
    OesdkHeapFirstBlock->Size = Available - sizeof(OesdkHeapBlock);
    OesdkHeapFirstBlock->Free = true;
    OesdkHeapFirstBlock->Previous = NULL;
    OesdkHeapFirstBlock->Next = NULL;

    OesdkHeapState.Base = AlignedStart;
    OesdkHeapState.Current = AlignedStart;
    OesdkHeapState.End = End;
    OesdkHeapState.Capacity = Available - sizeof(OesdkHeapBlock);
    OesdkHeapState.Used = 0U;
    OesdkHeapState.Remaining = OesdkHeapState.Capacity;
    OesdkHeapState.AllocationCount = 0U;
    OesdkHeapState.ActiveAllocationCount = 0U;
    OesdkHeapState.FreeBlockCount = 1U;
    OesdkHeapState.LargestFreeBlock = OesdkHeapState.Capacity;
    OesdkHeapState.Initialized = true;
    OesdkHeapState.PermanentInitialized = true;
    return OESDK_STATUS_SUCCESS;
}

void *OesdkAllocateAligned(size_t Size, size_t Alignment)
{
    OesdkHeapBlock *Block;
    if (!OesdkHeapState.PermanentInitialized || Size == 0U || !IsPowerOfTwo(Alignment)) return NULL;
    if (Alignment < OESDK_HEAP_DEFAULT_ALIGNMENT) Alignment = OESDK_HEAP_DEFAULT_ALIGNMENT;

    Block = OesdkHeapFirstBlock;
    while (BlockValid(Block)) {
        if (Block->Free) {
            uintptr_t OriginalAddress = BlockAddress(Block);
            uintptr_t OriginalEnd = BlockEndAddress(Block);
            uintptr_t Payload;
            uintptr_t Header;
            uintptr_t RequiredEnd;
            if (!AlignUp(BlockDataAddress(Block), Alignment, &Payload)) return NULL;
            Header = Payload - sizeof(OesdkHeapBlock);
            if (Header != OriginalAddress && Header - OriginalAddress < sizeof(OesdkHeapBlock) + OESDK_HEAP_MINIMUM_BLOCK_SIZE) {
                uintptr_t NextCandidate;
                if (AddOverflow(Payload, (uintptr_t)Alignment, &NextCandidate)) return NULL;
                Payload = NextCandidate;
                Header = Payload - sizeof(OesdkHeapBlock);
            }
            if (Header < OriginalAddress || AddOverflow(Payload, (uintptr_t)Size, &RequiredEnd)) return NULL;
            if (RequiredEnd <= OriginalEnd) {
                OesdkHeapBlock *Allocated = CreateAllocatedBlock(Block, Header, Size);
                OesdkHeapState.AllocationCount++;
                RefreshPermanentStatistics();
                return (void *)BlockDataAddress(Allocated);
            }
        }
        Block = Block->Next;
    }
    return NULL;
}

void *OesdkAllocate(size_t Size)
{
    return OesdkAllocateAligned(Size, OESDK_HEAP_DEFAULT_ALIGNMENT);
}

void *OesdkAllocateZeroed(size_t Count, size_t Size)
{
    size_t Total;
    unsigned char *Memory;
    size_t Index;
    if (Count != 0U && Size > ((size_t)-1) / Count) return NULL;
    Total = Count * Size;
    Memory = (unsigned char *)OesdkAllocate(Total);
    if (Memory == NULL) return NULL;
    for (Index = 0U; Index < Total; ++Index) Memory[Index] = 0U;
    return Memory;
}

void OesdkFree(void *Address)
{
    OesdkHeapBlock *Block;
    uintptr_t Pointer;
    if (!OesdkHeapState.PermanentInitialized || Address == NULL) return;
    Pointer = (uintptr_t)Address;
    if (Pointer < OesdkHeapState.Base + sizeof(OesdkHeapBlock) || Pointer >= OesdkHeapState.End) return;
    Block = (OesdkHeapBlock *)(Pointer - sizeof(OesdkHeapBlock));
    if (!BlockValid(Block) || Block->Free || BlockDataAddress(Block) != Pointer) return;
    Block->Free = true;
    MergeWithNext(Block);
    if (BlockValid(Block->Previous) && Block->Previous->Free && BlocksAreContiguous(Block->Previous, Block)) {
        Block = Block->Previous;
        MergeWithNext(Block);
    }
    RefreshPermanentStatistics();
}

bool OesdkHeapIsInitialized(void)
{
    return OesdkHeapState.PermanentInitialized;
}

const OesdkHeapInformation *OesdkHeapInformationGet(void)
{
    return &OesdkHeapState;
}
