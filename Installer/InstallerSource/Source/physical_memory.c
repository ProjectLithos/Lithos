#include <oesdk/physical_memory.h>
#include <string.h>

#define BITMAP_CAPACITY_BYTES ((size_t)((OESDK_PHYSICAL_MAX_PAGES + 7ULL) / 8ULL))

extern unsigned char __image_start[];
extern unsigned char __bss_end[];
extern unsigned char __bootstrap_stack_bottom[];
extern unsigned char __bootstrap_stack_top[];
extern unsigned char __bootstrap_page_tables_start[];
extern unsigned char __bootstrap_page_tables_end[];

static unsigned char AllocationBitmap[BITMAP_CAPACITY_BYTES];
static unsigned char PermanentBitmap[BITMAP_CAPACITY_BYTES];
static OesdkPhysicalMemoryInformation Information;
static bool Initialized;

static bool AddU64(uint64_t Left, uint64_t Right, uint64_t *Result)
{
    if (Result == NULL || UINT64_MAX - Left < Right) return false;
    *Result = Left + Right;
    return true;
}

static bool PageInRange(uint64_t PageNumber)
{
    return PageNumber < Information.PageCount;
}

static unsigned char PageMask(uint64_t PageNumber)
{
    return (unsigned char)(1U << (unsigned int)(PageNumber & 7ULL));
}

static bool BitmapGet(const unsigned char *Bitmap, uint64_t PageNumber)
{
    return (Bitmap[(size_t)(PageNumber >> 3U)] & PageMask(PageNumber)) != 0U;
}

static void BitmapSet(unsigned char *Bitmap, uint64_t PageNumber, bool Value)
{
    size_t ByteIndex = (size_t)(PageNumber >> 3U);
    unsigned char Mask = PageMask(PageNumber);
    if (Value) Bitmap[ByteIndex] |= Mask;
    else Bitmap[ByteIndex] &= (unsigned char)~Mask;
}

bool OesdkAlignmentIsPowerOfTwo(uint64_t Value)
{
    return Value != 0U && (Value & (Value - 1U)) == 0U;
}

bool OesdkAlignDownU64(uint64_t Value, uint64_t Alignment, uint64_t *Result)
{
    if (Result == NULL || !OesdkAlignmentIsPowerOfTwo(Alignment)) return false;
    *Result = Value & ~(Alignment - 1U);
    return true;
}

bool OesdkAlignUpU64(uint64_t Value, uint64_t Alignment, uint64_t *Result)
{
    uint64_t Adjustment;
    if (Result == NULL || !OesdkAlignmentIsPowerOfTwo(Alignment)) return false;
    Adjustment = Alignment - 1U;
    if (UINT64_MAX - Value < Adjustment) return false;
    *Result = (Value + Adjustment) & ~Adjustment;
    return true;
}

bool OesdkPhysicalAddressToPage(uint64_t PhysicalAddress, uint64_t *PageNumber)
{
    if (PageNumber == NULL) return false;
    *PageNumber = PhysicalAddress >> OESDK_PAGE_SHIFT;
    return true;
}

bool OesdkPhysicalPageToAddress(uint64_t PageNumber, uint64_t *PhysicalAddress)
{
    if (PhysicalAddress == NULL || PageNumber > (UINT64_MAX >> OESDK_PAGE_SHIFT)) return false;
    *PhysicalAddress = PageNumber << OESDK_PAGE_SHIFT;
    return true;
}

static void SetPageAllocated(uint64_t PageNumber, bool Allocated)
{
    bool WasAllocated;
    if (!PageInRange(PageNumber)) return;
    WasAllocated = BitmapGet(AllocationBitmap, PageNumber);
    if (WasAllocated == Allocated) return;
    BitmapSet(AllocationBitmap, PageNumber, Allocated);
    if (Allocated)
    {
        --Information.FreePageCount;
        ++Information.ReservedPageCount;
    }
    else
    {
        ++Information.FreePageCount;
        --Information.ReservedPageCount;
    }
}

static OesdkStatus MarkAvailable(uint64_t Base, uint64_t Length)
{
    uint64_t End;
    uint64_t First;
    uint64_t Last;
    uint64_t Page;
    if (Length == 0U || !AddU64(Base, Length, &End)) return OESDK_STATUS_CORRUPT;
    if (!OesdkAlignUpU64(Base, OESDK_PAGE_SIZE, &First)) return OESDK_STATUS_CORRUPT;
    if (!OesdkAlignDownU64(End, OESDK_PAGE_SIZE, &Last)) return OESDK_STATUS_CORRUPT;
    if (First >= Last) return OESDK_STATUS_SUCCESS;
    for (Page = First >> OESDK_PAGE_SHIFT; Page < (Last >> OESDK_PAGE_SHIFT) && Page < Information.PageCount; ++Page)
        SetPageAllocated(Page, false);
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkPhysicalMemoryReserve(uint64_t Base, uint64_t Length, bool Permanent)
{
    uint64_t End;
    uint64_t FirstPage;
    uint64_t LastPage;
    uint64_t Page;
    if (!Initialized || Length == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    if (!AddU64(Base, Length, &End)) return OESDK_STATUS_CORRUPT;
    FirstPage = Base >> OESDK_PAGE_SHIFT;
    LastPage = (End >> OESDK_PAGE_SHIFT) + ((End & OESDK_PAGE_MASK) != 0U ? 1U : 0U);
    if (FirstPage >= Information.PageCount) return OESDK_STATUS_NOT_FOUND;
    if (LastPage > Information.PageCount) LastPage = Information.PageCount;
    for (Page = FirstPage; Page < LastPage; ++Page)
    {
        SetPageAllocated(Page, true);
        if (Permanent) BitmapSet(PermanentBitmap, Page, true);
    }
    return OESDK_STATUS_SUCCESS;
}

static OesdkStatus ReserveAddressRange(uintptr_t Start, uintptr_t End)
{
    if (End <= Start) return OESDK_STATUS_CORRUPT;
    return OesdkPhysicalMemoryReserve((uint64_t)Start, (uint64_t)(End - Start), true);
}

static OesdkStatus ReserveMultibootStructures(const OesdkBootContext *BootContext)
{
    typedef struct __attribute__((packed)) MultibootInformation
    {
        uint32_t Flags, MemoryLower, MemoryUpper, BootDevice, CommandLine;
        uint32_t ModulesCount, ModulesAddress, Symbols[4], MemoryMapLength, MemoryMapAddress;
        uint32_t DrivesLength, DrivesAddress, ConfigurationTable, BootLoaderName, ApmTable;
        uint32_t VbeControlInfo, VbeModeInfo;
        uint16_t VbeMode, VbeInterfaceSegment, VbeInterfaceOffset, VbeInterfaceLength;
        uint64_t FramebufferAddress;
        uint32_t FramebufferPitch, FramebufferWidth, FramebufferHeight;
        uint8_t FramebufferBitsPerPixel, FramebufferType;
        uint16_t Reserved;
    } MultibootInformation;
    const MultibootInformation *Info;
    OesdkStatus Status;
    uint64_t FramebufferBytes;
    if (BootContext == NULL || BootContext->BootInformationAddress == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    Info = (const MultibootInformation *)BootContext->BootInformationAddress;
    Status = OesdkPhysicalMemoryReserve((uint64_t)BootContext->BootInformationAddress, sizeof(*Info), true);
    if (OESDK_FAILED(Status) && Status != OESDK_STATUS_NOT_FOUND) return Status;
    if ((Info->Flags & (1U << 6)) != 0U && Info->MemoryMapAddress != 0U && Info->MemoryMapLength != 0U)
    {
        Status = OesdkPhysicalMemoryReserve(Info->MemoryMapAddress, Info->MemoryMapLength, true);
        if (OESDK_FAILED(Status) && Status != OESDK_STATUS_NOT_FOUND) return Status;
    }
    if ((Info->Flags & (1U << 3)) != 0U && Info->ModulesAddress != 0U && Info->ModulesCount != 0U)
    {
        uint64_t Bytes = (uint64_t)Info->ModulesCount * 16ULL;
        if (Bytes / 16ULL != Info->ModulesCount) return OESDK_STATUS_CORRUPT;
        Status = OesdkPhysicalMemoryReserve(Info->ModulesAddress, Bytes, true);
        if (OESDK_FAILED(Status) && Status != OESDK_STATUS_NOT_FOUND) return Status;
    }
    if ((Info->Flags & (1U << 12)) != 0U && Info->FramebufferAddress != 0U && Info->FramebufferPitch != 0U && Info->FramebufferHeight != 0U)
    {
        FramebufferBytes = (uint64_t)Info->FramebufferPitch * (uint64_t)Info->FramebufferHeight;
        if (FramebufferBytes / Info->FramebufferPitch != Info->FramebufferHeight) return OESDK_STATUS_CORRUPT;
        Status = OesdkPhysicalMemoryReserve(Info->FramebufferAddress, FramebufferBytes, true);
        if (OESDK_FAILED(Status) && Status != OESDK_STATUS_NOT_FOUND) return Status;
    }
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkPhysicalMemoryInitialize(
    const OesdkMemoryRegion *Regions,
    size_t RegionCount,
    const OesdkBootContext *BootContext)
{
    uint64_t HighestEnd = 0U;
    size_t Index;
    OesdkStatus Status;
    if (Regions == NULL || RegionCount == 0U || BootContext == NULL) return OESDK_STATUS_INVALID_ARGUMENT;
    memset(&Information, 0, sizeof(Information));
    memset(AllocationBitmap, 0xFF, sizeof(AllocationBitmap));
    memset(PermanentBitmap, 0, sizeof(PermanentBitmap));
    for (Index = 0U; Index < RegionCount; ++Index)
    {
        uint64_t End;
        if (!OesdkMemoryRangeEnd(&Regions[Index], &End)) return OESDK_STATUS_CORRUPT;
        if (End > HighestEnd) HighestEnd = End;
    }
    if (HighestEnd > OESDK_PHYSICAL_MAX_BYTES) HighestEnd = OESDK_PHYSICAL_MAX_BYTES;
    if (!OesdkAlignUpU64(HighestEnd, OESDK_PAGE_SIZE, &HighestEnd)) return OESDK_STATUS_CORRUPT;
    Information.PageSize = OESDK_PAGE_SIZE;
    Information.PageCount = HighestEnd >> OESDK_PAGE_SHIFT;
    Information.BitmapBytes = (Information.PageCount + 7U) / 8U;
    Information.ReservedPageCount = Information.PageCount;
    Information.HighestPhysicalAddress = HighestEnd;
    if (Information.PageCount == 0U || Information.PageCount > OESDK_PHYSICAL_MAX_PAGES) return OESDK_STATUS_OUT_OF_MEMORY;
    Initialized = true;
    for (Index = 0U; Index < RegionCount; ++Index)
        if (Regions[Index].Type == OesdkMemoryAvailable)
        {
            Status = MarkAvailable(Regions[Index].Base, Regions[Index].Length);
            if (OESDK_FAILED(Status)) return Status;
        }
    Status = OesdkPhysicalMemoryReserve(0U, OESDK_PAGE_SIZE, true);
    if (OESDK_FAILED(Status)) return Status;
    Status = ReserveAddressRange((uintptr_t)__image_start, (uintptr_t)__bss_end);
    if (OESDK_FAILED(Status)) return Status;
    Status = ReserveAddressRange((uintptr_t)__bootstrap_stack_bottom, (uintptr_t)__bootstrap_stack_top);
    if (OESDK_FAILED(Status)) return Status;
    Status = ReserveAddressRange((uintptr_t)__bootstrap_page_tables_start, (uintptr_t)__bootstrap_page_tables_end);
    if (OESDK_FAILED(Status)) return Status;
    Status = OesdkPhysicalMemoryReserve((uint64_t)(uintptr_t)AllocationBitmap, sizeof(AllocationBitmap), true);
    if (OESDK_FAILED(Status)) return Status;
    Status = OesdkPhysicalMemoryReserve((uint64_t)(uintptr_t)PermanentBitmap, sizeof(PermanentBitmap), true);
    if (OESDK_FAILED(Status)) return Status;
    Status = ReserveMultibootStructures(BootContext);
    if (OESDK_FAILED(Status)) return Status;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkPhysicalMemoryAllocate(size_t PageCount, uint64_t *PhysicalAddress)
{
    uint64_t Start;
    uint64_t Run = 0U;
    uint64_t Page;
    if (!Initialized || PhysicalAddress == NULL || PageCount == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    if ((uint64_t)PageCount > Information.FreePageCount) return OESDK_STATUS_OUT_OF_MEMORY;
    Start = 0U;
    for (Page = 0U; Page < Information.PageCount; ++Page)
    {
        if (!BitmapGet(AllocationBitmap, Page))
        {
            if (Run == 0U) Start = Page;
            ++Run;
            if (Run == (uint64_t)PageCount)
            {
                uint64_t Current;
                for (Current = Start; Current < Start + Run; ++Current) SetPageAllocated(Current, true);
                return OesdkPhysicalPageToAddress(Start, PhysicalAddress) ? OESDK_STATUS_SUCCESS : OESDK_STATUS_CORRUPT;
            }
        }
        else Run = 0U;
    }
    return OESDK_STATUS_OUT_OF_MEMORY;
}

OesdkStatus OesdkPhysicalMemoryFree(uint64_t PhysicalAddress, size_t PageCount)
{
    uint64_t FirstPage;
    uint64_t Page;
    if (!Initialized || PageCount == 0U || (PhysicalAddress & OESDK_PAGE_MASK) != 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    FirstPage = PhysicalAddress >> OESDK_PAGE_SHIFT;
    if (FirstPage >= Information.PageCount || (uint64_t)PageCount > Information.PageCount - FirstPage) return OESDK_STATUS_NOT_FOUND;
    for (Page = FirstPage; Page < FirstPage + (uint64_t)PageCount; ++Page)
        if (BitmapGet(PermanentBitmap, Page)) return OESDK_STATUS_BUSY;
    for (Page = FirstPage; Page < FirstPage + (uint64_t)PageCount; ++Page)
        SetPageAllocated(Page, false);
    return OESDK_STATUS_SUCCESS;
}

bool OesdkPhysicalMemoryPageIsAllocated(uint64_t PageNumber)
{
    return Initialized && PageInRange(PageNumber) && BitmapGet(AllocationBitmap, PageNumber);
}

bool OesdkPhysicalMemoryPageIsPermanent(uint64_t PageNumber)
{
    return Initialized && PageInRange(PageNumber) && BitmapGet(PermanentBitmap, PageNumber);
}

bool OesdkPhysicalMemoryIsInitialized(void)
{
    return Initialized;
}

const OesdkPhysicalMemoryInformation *OesdkPhysicalMemoryInformationGet(void)
{
    return Initialized ? &Information : NULL;
}
