#include <oesdk/virtual_memory.h>
#include <oesdk/cpu.h>
#include <oesdk/physical_memory.h>
#include <string.h>

#define ENTRY_COUNT 512U
#define ENTRY_PRESENT (1ULL << 0)
#define ENTRY_WRITABLE (1ULL << 1)
#define ENTRY_USER (1ULL << 2)
#define ENTRY_WRITE_THROUGH (1ULL << 3)
#define ENTRY_CACHE_DISABLE (1ULL << 4)
#define ENTRY_LARGE_PAGE (1ULL << 7)
#define ENTRY_GLOBAL (1ULL << 8)
#define ENTRY_NO_EXECUTE (1ULL << 63)
#define ENTRY_ADDRESS_MASK 0x000FFFFFFFFFF000ULL
#define ONE_GIB_MASK ((1ULL << 30) - 1ULL)
#define TWO_MIB_MASK ((1ULL << 21) - 1ULL)
#define EFER_MSR 0xC0000080U
#define EFER_NXE (1ULL << 11)

typedef uint64_t OesdkPageTable[ENTRY_COUNT];

static OesdkVirtualMemoryInformation Information;
static OesdkPageTable *Root;

static uint64_t ReadCr3(void)
{
    uint64_t Value;
    __asm__ volatile("mov %%cr3, %0" : "=r"(Value));
    return Value;
}

static uint64_t ReadMsr(uint32_t Register)
{
    uint32_t Low;
    uint32_t High;
    __asm__ volatile("rdmsr" : "=a"(Low), "=d"(High) : "c"(Register));
    return ((uint64_t)High << 32U) | Low;
}

static void WriteMsr(uint32_t Register, uint64_t Value)
{
    __asm__ volatile("wrmsr" : : "c"(Register), "a"((uint32_t)Value), "d"((uint32_t)(Value >> 32U)) : "memory");
}

static void Invalidate(uintptr_t VirtualAddress)
{
    __asm__ volatile("invlpg (%0)" : : "r"(VirtualAddress) : "memory");
}

static OesdkPageTable *PhysicalTable(uint64_t PhysicalAddress)
{
    /* The bootstrap keeps low physical memory identity-mapped. */
    return (OesdkPageTable *)(uintptr_t)(PhysicalAddress & ENTRY_ADDRESS_MASK);
}

static bool AddRange(uintptr_t Address, size_t PageCount, uintptr_t *LastAddress)
{
    uint64_t Bytes;
    if (LastAddress == NULL || PageCount == 0U) return false;
    if ((uint64_t)PageCount > UINT64_MAX / OESDK_VIRTUAL_PAGE_SIZE) return false;
    Bytes = (uint64_t)PageCount * OESDK_VIRTUAL_PAGE_SIZE;
    if (Bytes == 0U || UINTPTR_MAX - Address < (uintptr_t)(Bytes - 1U)) return false;
    *LastAddress = Address + (uintptr_t)(Bytes - 1U);
    return true;
}

bool OesdkVirtualAddressIsCanonical(uintptr_t VirtualAddress)
{
    uint64_t Value = (uint64_t)VirtualAddress;
    uint64_t Upper = Value >> 48U;
    return ((Value & (1ULL << 47U)) == 0U) ? Upper == 0U : Upper == 0xFFFFU;
}

static uint64_t LeafFlags(OesdkVirtualFlags Flags)
{
    uint64_t Result = ENTRY_PRESENT;
    if ((Flags & OESDK_VIRTUAL_FLAG_WRITABLE) != 0U) Result |= ENTRY_WRITABLE;
    if ((Flags & OESDK_VIRTUAL_FLAG_USER) != 0U) Result |= ENTRY_USER;
    if ((Flags & OESDK_VIRTUAL_FLAG_WRITE_THROUGH) != 0U) Result |= ENTRY_WRITE_THROUGH;
    if ((Flags & OESDK_VIRTUAL_FLAG_CACHE_DISABLE) != 0U) Result |= ENTRY_CACHE_DISABLE;
    if ((Flags & OESDK_VIRTUAL_FLAG_GLOBAL) != 0U) Result |= ENTRY_GLOBAL;
    if ((Flags & OESDK_VIRTUAL_FLAG_NO_EXECUTE) != 0U) Result |= ENTRY_NO_EXECUTE;
    return Result;
}

static uint64_t ParentFlags(OesdkVirtualFlags Flags)
{
    uint64_t Result = ENTRY_PRESENT | ENTRY_WRITABLE;
    if ((Flags & OESDK_VIRTUAL_FLAG_USER) != 0U) Result |= ENTRY_USER;
    return Result;
}

static OesdkStatus AllocateTable(OesdkPageTable **Table, uint64_t *PhysicalAddress)
{
    OesdkStatus Status;
    if (Table == NULL || PhysicalAddress == NULL) return OESDK_STATUS_INVALID_ARGUMENT;
    Status = OesdkPhysicalMemoryAllocate(1U, PhysicalAddress);
    if (OESDK_FAILED(Status)) return Status;
    /* Current bootstrap mapping only makes the first 4 GiB directly addressable. */
    if (*PhysicalAddress >= 0x100000000ULL)
    {
        (void)OesdkPhysicalMemoryFree(*PhysicalAddress, 1U);
        return OESDK_STATUS_NOT_SUPPORTED;
    }
    *Table = PhysicalTable(*PhysicalAddress);
    memset(*Table, 0, OESDK_VIRTUAL_PAGE_SIZE);
    ++Information.AllocatedTableCount;
    return OESDK_STATUS_SUCCESS;
}

static OesdkStatus SplitTwoMibEntry(uint64_t *Entry)
{
    OesdkPageTable *Table;
    uint64_t TablePhysical;
    uint64_t Base;
    uint64_t Common;
    unsigned int Index;
    OesdkStatus Status;
    if (Entry == NULL || (*Entry & (ENTRY_PRESENT | ENTRY_LARGE_PAGE)) != (ENTRY_PRESENT | ENTRY_LARGE_PAGE))
        return OESDK_STATUS_INVALID_ARGUMENT;
    Status = AllocateTable(&Table, &TablePhysical);
    if (OESDK_FAILED(Status)) return Status;
    Base = *Entry & 0x000FFFFFFFE00000ULL;
    Common = *Entry & (ENTRY_WRITABLE | ENTRY_USER | ENTRY_WRITE_THROUGH | ENTRY_CACHE_DISABLE | ENTRY_GLOBAL | ENTRY_NO_EXECUTE);
    for (Index = 0U; Index < ENTRY_COUNT; ++Index)
        (*Table)[Index] = (Base + ((uint64_t)Index << 12U)) | ENTRY_PRESENT | Common;
    *Entry = (TablePhysical & ENTRY_ADDRESS_MASK) | ENTRY_PRESENT | ENTRY_WRITABLE | ((*Entry) & ENTRY_USER);
    return OESDK_STATUS_SUCCESS;
}

static OesdkStatus GetLeaf(uintptr_t VirtualAddress, OesdkVirtualFlags Flags, bool Create, uint64_t **Leaf)
{
    unsigned int Pml4Index = (unsigned int)(((uint64_t)VirtualAddress >> 39U) & 0x1FFU);
    unsigned int PdptIndex = (unsigned int)(((uint64_t)VirtualAddress >> 30U) & 0x1FFU);
    unsigned int PdIndex = (unsigned int)(((uint64_t)VirtualAddress >> 21U) & 0x1FFU);
    unsigned int PtIndex = (unsigned int)(((uint64_t)VirtualAddress >> 12U) & 0x1FFU);
    OesdkPageTable *Pml4 = Root;
    OesdkPageTable *Pdpt;
    OesdkPageTable *Pd;
    OesdkPageTable *Pt;
    OesdkStatus Status;
    uint64_t Physical;
    uint64_t Parent = ParentFlags(Flags);
    if (Leaf == NULL || Root == NULL) return OESDK_STATUS_INVALID_ARGUMENT;

    if (((*Pml4)[Pml4Index] & ENTRY_PRESENT) == 0U)
    {
        if (!Create) return OESDK_STATUS_NOT_FOUND;
        Status = AllocateTable(&Pdpt, &Physical);
        if (OESDK_FAILED(Status)) return Status;
        (*Pml4)[Pml4Index] = Physical | Parent;
    }
    else Pdpt = PhysicalTable((*Pml4)[Pml4Index]);
    if (((*Pml4)[Pml4Index] & ENTRY_USER) == 0U && (Flags & OESDK_VIRTUAL_FLAG_USER) != 0U) (*Pml4)[Pml4Index] |= ENTRY_USER;

    Pdpt = PhysicalTable((*Pml4)[Pml4Index]);
    if (((*Pdpt)[PdptIndex] & ENTRY_LARGE_PAGE) != 0U) return OESDK_STATUS_NOT_SUPPORTED;
    if (((*Pdpt)[PdptIndex] & ENTRY_PRESENT) == 0U)
    {
        if (!Create) return OESDK_STATUS_NOT_FOUND;
        Status = AllocateTable(&Pd, &Physical);
        if (OESDK_FAILED(Status)) return Status;
        (*Pdpt)[PdptIndex] = Physical | Parent;
    }
    if (((*Pdpt)[PdptIndex] & ENTRY_USER) == 0U && (Flags & OESDK_VIRTUAL_FLAG_USER) != 0U) (*Pdpt)[PdptIndex] |= ENTRY_USER;

    Pd = PhysicalTable((*Pdpt)[PdptIndex]);
    if (((*Pd)[PdIndex] & ENTRY_LARGE_PAGE) != 0U)
    {
        if (!Create) return OESDK_STATUS_BUSY;
        Status = SplitTwoMibEntry(&(*Pd)[PdIndex]);
        if (OESDK_FAILED(Status)) return Status;
    }
    if (((*Pd)[PdIndex] & ENTRY_PRESENT) == 0U)
    {
        if (!Create) return OESDK_STATUS_NOT_FOUND;
        Status = AllocateTable(&Pt, &Physical);
        if (OESDK_FAILED(Status)) return Status;
        (*Pd)[PdIndex] = Physical | Parent;
    }
    if (((*Pd)[PdIndex] & ENTRY_USER) == 0U && (Flags & OESDK_VIRTUAL_FLAG_USER) != 0U) (*Pd)[PdIndex] |= ENTRY_USER;

    Pt = PhysicalTable((*Pd)[PdIndex]);
    *Leaf = &(*Pt)[PtIndex];
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkVirtualMemoryInitialize(void)
{
    uint64_t RootPhysical;
    const OesdkCpuFeatures *Features;
    if (!OesdkPhysicalMemoryIsInitialized()) return OESDK_STATUS_BUSY;
    RootPhysical = ReadCr3() & ENTRY_ADDRESS_MASK;
    if (RootPhysical == 0U || RootPhysical >= 0x100000000ULL) return OESDK_STATUS_NOT_SUPPORTED;
    memset(&Information, 0, sizeof(Information));
    Root = PhysicalTable(RootPhysical);
    Information.RootPhysicalAddress = RootPhysical;
    Features = OesdkCpuFeaturesGet();
    if (Features != NULL && Features->Nx)
    {
        uint64_t Efer = ReadMsr(EFER_MSR);
        if ((Efer & EFER_NXE) == 0U) WriteMsr(EFER_MSR, Efer | EFER_NXE);
        Information.NoExecuteEnabled = true;
    }
    Information.Initialized = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkVirtualMap(uintptr_t VirtualAddress, uintptr_t PhysicalAddress, size_t PageCount, OesdkVirtualFlags Flags)
{
    uintptr_t VirtualLast;
    uintptr_t PhysicalLast;
    size_t Index;
    uint64_t *Leaf;
    OesdkStatus Status;
    if (!Information.Initialized || PageCount == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    if ((((uint64_t)VirtualAddress | (uint64_t)PhysicalAddress) & OESDK_VIRTUAL_PAGE_MASK) != 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    if (!AddRange(VirtualAddress, PageCount, &VirtualLast) || !AddRange(PhysicalAddress, PageCount, &PhysicalLast)) return OESDK_STATUS_CORRUPT;
    if (!OesdkVirtualAddressIsCanonical(VirtualAddress) || !OesdkVirtualAddressIsCanonical(VirtualLast)) return OESDK_STATUS_INVALID_ARGUMENT;
    if ((Flags & OESDK_VIRTUAL_FLAG_NO_EXECUTE) != 0U && !Information.NoExecuteEnabled) return OESDK_STATUS_NOT_SUPPORTED;
    for (Index = 0U; Index < PageCount; ++Index)
    {
        uintptr_t Va = VirtualAddress + Index * OESDK_VIRTUAL_PAGE_SIZE;
        uintptr_t Pa = PhysicalAddress + Index * OESDK_VIRTUAL_PAGE_SIZE;
        Status = GetLeaf(Va, Flags, true, &Leaf);
        if (OESDK_FAILED(Status)) return Status;
        if ((*Leaf & ENTRY_PRESENT) != 0U) return OESDK_STATUS_ALREADY_EXISTS;
        *Leaf = ((uint64_t)Pa & ENTRY_ADDRESS_MASK) | LeafFlags(Flags);
        Invalidate(Va);
        ++Information.MappedPageCount;
    }
    (void)PhysicalLast;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkVirtualUnmap(uintptr_t VirtualAddress, size_t PageCount)
{
    uintptr_t VirtualLast;
    size_t Index;
    uint64_t *Leaf;
    OesdkStatus Status;
    if (!Information.Initialized || PageCount == 0U || ((uint64_t)VirtualAddress & OESDK_VIRTUAL_PAGE_MASK) != 0U)
        return OESDK_STATUS_INVALID_ARGUMENT;
    if (!AddRange(VirtualAddress, PageCount, &VirtualLast) || !OesdkVirtualAddressIsCanonical(VirtualAddress) || !OesdkVirtualAddressIsCanonical(VirtualLast))
        return OESDK_STATUS_CORRUPT;
    for (Index = 0U; Index < PageCount; ++Index)
    {
        uintptr_t Va = VirtualAddress + Index * OESDK_VIRTUAL_PAGE_SIZE;
        Status = GetLeaf(Va, 0U, false, &Leaf);
        if (Status == OESDK_STATUS_BUSY) return OESDK_STATUS_NOT_SUPPORTED;
        if (OESDK_FAILED(Status) || (*Leaf & ENTRY_PRESENT) == 0U) return OESDK_STATUS_NOT_FOUND;
        *Leaf = 0U;
        Invalidate(Va);
        ++Information.UnmappedPageCount;
    }
    return OESDK_STATUS_SUCCESS;
}

bool OesdkVirtualTranslate(uintptr_t VirtualAddress, uintptr_t *PhysicalAddress)
{
    unsigned int Pml4Index;
    unsigned int PdptIndex;
    unsigned int PdIndex;
    unsigned int PtIndex;
    OesdkPageTable *Pdpt;
    OesdkPageTable *Pd;
    OesdkPageTable *Pt;
    uint64_t Entry;
    if (!Information.Initialized || PhysicalAddress == NULL || !OesdkVirtualAddressIsCanonical(VirtualAddress)) return false;
    Pml4Index = (unsigned int)(((uint64_t)VirtualAddress >> 39U) & 0x1FFU);
    PdptIndex = (unsigned int)(((uint64_t)VirtualAddress >> 30U) & 0x1FFU);
    PdIndex = (unsigned int)(((uint64_t)VirtualAddress >> 21U) & 0x1FFU);
    PtIndex = (unsigned int)(((uint64_t)VirtualAddress >> 12U) & 0x1FFU);
    Entry = (*Root)[Pml4Index];
    if ((Entry & ENTRY_PRESENT) == 0U) return false;
    Pdpt = PhysicalTable(Entry);
    Entry = (*Pdpt)[PdptIndex];
    if ((Entry & ENTRY_PRESENT) == 0U) return false;
    if ((Entry & ENTRY_LARGE_PAGE) != 0U)
    {
        *PhysicalAddress = (uintptr_t)((Entry & 0x000FFFFFC0000000ULL) | ((uint64_t)VirtualAddress & ONE_GIB_MASK));
        return true;
    }
    Pd = PhysicalTable(Entry);
    Entry = (*Pd)[PdIndex];
    if ((Entry & ENTRY_PRESENT) == 0U) return false;
    if ((Entry & ENTRY_LARGE_PAGE) != 0U)
    {
        *PhysicalAddress = (uintptr_t)((Entry & 0x000FFFFFFFE00000ULL) | ((uint64_t)VirtualAddress & TWO_MIB_MASK));
        return true;
    }
    Pt = PhysicalTable(Entry);
    Entry = (*Pt)[PtIndex];
    if ((Entry & ENTRY_PRESENT) == 0U) return false;
    *PhysicalAddress = (uintptr_t)((Entry & ENTRY_ADDRESS_MASK) | ((uint64_t)VirtualAddress & OESDK_VIRTUAL_PAGE_MASK));
    return true;
}

bool OesdkVirtualMemoryIsInitialized(void)
{
    return Information.Initialized;
}

const OesdkVirtualMemoryInformation *OesdkVirtualMemoryInformationGet(void)
{
    return Information.Initialized ? &Information : NULL;
}
