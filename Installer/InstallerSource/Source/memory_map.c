#include <oesdk/memory_map.h>

#include <string.h>

typedef struct __attribute__((packed)) OesdkMultiboot1MemoryMapEntry
{
    uint32_t Size;
    uint64_t Base;
    uint64_t Length;
    uint32_t Type;
} OesdkMultiboot1MemoryMapEntry;

static OesdkMemoryRegion Regions[OESDK_MEMORY_MAP_MAX_REGIONS];
static size_t RegionCount;

bool OesdkMemoryRangeEnd(const OesdkMemoryRegion *Region, uint64_t *End)
{
    if (Region == NULL || End == NULL || Region->Length == 0U) return false;
    if (UINT64_MAX - Region->Base < Region->Length) return false;
    *End = Region->Base + Region->Length;
    return *End >= Region->Base;
}

bool OesdkMemoryRangeIsValid(const OesdkMemoryRegion *Region)
{
    uint64_t End;
    return OesdkMemoryRangeEnd(Region, &End);
}

bool OesdkMemoryRangesOverlap(const OesdkMemoryRegion *Left, const OesdkMemoryRegion *Right)
{
    uint64_t LeftEnd;
    uint64_t RightEnd;
    if (!OesdkMemoryRangeEnd(Left, &LeftEnd) || !OesdkMemoryRangeEnd(Right, &RightEnd)) return false;
    return Left->Base < RightEnd && Right->Base < LeftEnd;
}

OesdkMemoryType OesdkMemoryTypeFromMultiboot1(uint32_t BootloaderType)
{
    switch (BootloaderType)
    {
        case 1U: return OesdkMemoryAvailable;
        case 3U: return OesdkMemoryAcpiReclaimable;
        case 4U: return OesdkMemoryAcpiNvs;
        case 5U: return OesdkMemoryBad;
        default: return OesdkMemoryReserved;
    }
}

static void SortRegions(void)
{
    size_t Index;
    for (Index = 1U; Index < RegionCount; ++Index)
    {
        OesdkMemoryRegion Value = Regions[Index];
        size_t Position = Index;
        while (Position > 0U && Regions[Position - 1U].Base > Value.Base)
        {
            Regions[Position] = Regions[Position - 1U];
            --Position;
        }
        Regions[Position] = Value;
    }
}

static OesdkStatus CoalesceAndValidate(void)
{
    size_t ReadIndex;
    size_t WriteIndex = 0U;
    for (ReadIndex = 0U; ReadIndex < RegionCount; ++ReadIndex)
    {
        uint64_t CurrentEnd;
        if (!OesdkMemoryRangeEnd(&Regions[ReadIndex], &CurrentEnd)) return OESDK_STATUS_CORRUPT;
        if (WriteIndex != 0U)
        {
            OesdkMemoryRegion *Previous = &Regions[WriteIndex - 1U];
            uint64_t PreviousEnd;
            if (!OesdkMemoryRangeEnd(Previous, &PreviousEnd)) return OESDK_STATUS_CORRUPT;
            if (Regions[ReadIndex].Base < PreviousEnd) return OESDK_STATUS_CORRUPT;
            if (Regions[ReadIndex].Base == PreviousEnd && Regions[ReadIndex].Type == Previous->Type)
            {
                if (UINT64_MAX - Previous->Length < Regions[ReadIndex].Length) return OESDK_STATUS_CORRUPT;
                Previous->Length += Regions[ReadIndex].Length;
                continue;
            }
        }
        Regions[WriteIndex++] = Regions[ReadIndex];
    }
    RegionCount = WriteIndex;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkMemoryMapNormalizeMultiboot1(const void *MemoryMap, size_t MemoryMapBytes)
{
    const unsigned char *Cursor = (const unsigned char *)MemoryMap;
    size_t Remaining = MemoryMapBytes;
    RegionCount = 0U;
    memset(Regions, 0, sizeof(Regions));
    if (MemoryMap == NULL || MemoryMapBytes == 0U) return OESDK_STATUS_INVALID_ARGUMENT;

    while (Remaining != 0U)
    {
        const OesdkMultiboot1MemoryMapEntry *Entry;
        size_t RecordBytes;
        OesdkMemoryRegion Region;
        if (Remaining < sizeof(uint32_t)) return OESDK_STATUS_CORRUPT;
        Entry = (const OesdkMultiboot1MemoryMapEntry *)Cursor;
        if ((size_t)Entry->Size > ((size_t)-1) - sizeof(uint32_t)) return OESDK_STATUS_CORRUPT;
        RecordBytes = (size_t)Entry->Size + sizeof(uint32_t);
        if (RecordBytes < sizeof(OesdkMultiboot1MemoryMapEntry) || RecordBytes > Remaining) return OESDK_STATUS_CORRUPT;
        if (RegionCount >= OESDK_MEMORY_MAP_MAX_REGIONS) return OESDK_STATUS_OUT_OF_MEMORY;
        Region.Base = Entry->Base;
        Region.Length = Entry->Length;
        Region.Type = OesdkMemoryTypeFromMultiboot1(Entry->Type);
        if (!OesdkMemoryRangeIsValid(&Region)) return OESDK_STATUS_CORRUPT;
        Regions[RegionCount++] = Region;
        Cursor += RecordBytes;
        Remaining -= RecordBytes;
    }

    SortRegions();
    return CoalesceAndValidate();
}

const OesdkMemoryRegion *OesdkMemoryMapRegions(void)
{
    return Regions;
}

size_t OesdkMemoryMapRegionCount(void)
{
    return RegionCount;
}
