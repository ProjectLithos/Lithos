#include <oesdk/boot_context.h>

extern unsigned char __image_start[];
extern unsigned char __load_end[];
extern unsigned char __bss_end[];
extern unsigned char __bootstrap_stack_bottom[];
extern unsigned char __bootstrap_stack_top[];

typedef struct __attribute__((packed)) OesdkMultibootInformation {
    uint32_t Flags;
    uint32_t MemoryLowerKilobytes;
    uint32_t MemoryUpperKilobytes;
    uint32_t BootDevice;
    uint32_t CommandLine;
    uint32_t ModulesCount;
    uint32_t ModulesAddress;
    uint32_t Symbols[4];
    uint32_t MemoryMapLength;
    uint32_t MemoryMapAddress;
} OesdkMultibootInformation;

typedef struct __attribute__((packed)) OesdkMultibootMemoryMapEntry {
    uint32_t Size;
    uint64_t BaseAddress;
    uint64_t Length;
    uint32_t Type;
} OesdkMultibootMemoryMapEntry;

static OesdkBootContext Context;
static bool Initialized;

static bool AddU64(uint64_t Left, uint64_t Right, uint64_t *Result) {
    if (Result == NULL || UINT64_MAX - Left < Right) return false;
    *Result = Left + Right;
    return true;
}

static bool ReadAvailableMemory(const OesdkMultibootInformation *Information, uint64_t *Bytes) {
    if (Information == NULL || Bytes == NULL) return false;
    if ((Information->Flags & (1U << 6)) != 0U && Information->MemoryMapAddress != 0U) {
        uintptr_t Cursor = (uintptr_t)Information->MemoryMapAddress;
        uintptr_t End = Cursor + (uintptr_t)Information->MemoryMapLength;
        uint64_t Total = 0;
        if (End < Cursor) return false;
        while (Cursor < End) {
            if (End - Cursor < sizeof(uint32_t)) return false;
            const OesdkMultibootMemoryMapEntry *Entry = (const OesdkMultibootMemoryMapEntry *)Cursor;
            uintptr_t RecordSize = (uintptr_t)Entry->Size + sizeof(uint32_t);
            if (RecordSize < 24U || RecordSize > End - Cursor) return false;
            if (Entry->Type == 1U && !AddU64(Total, Entry->Length, &Total)) return false;
            Cursor += RecordSize;
        }
        *Bytes = Total;
        return true;
    }
    if ((Information->Flags & 1U) != 0U) {
        uint64_t Kilobytes = (uint64_t)Information->MemoryLowerKilobytes + (uint64_t)Information->MemoryUpperKilobytes;
        if (Kilobytes > UINT64_MAX / 1024U) return false;
        *Bytes = Kilobytes * 1024U;
        return true;
    }
    return false;
}

bool OesdkBootContextInitialize(uint32_t BootMagic, uintptr_t BootInformationAddress) {
    Context.StructureSize = (uint32_t)sizeof(Context);
    Context.AbiVersion = OESDK_BOOT_CONTEXT_ABI_VERSION;
    Context.BootProtocol = OesdkBootProtocolUnknown;
    Context.BootMagic = BootMagic;
    Context.BootInformationAddress = BootInformationAddress;
    Context.KernelPhysicalStart = (uintptr_t)__image_start;
    Context.KernelPhysicalEnd = (uintptr_t)__bss_end;
    Context.KernelLoadEnd = (uintptr_t)__load_end;
    Context.BootstrapStackStart = (uintptr_t)__bootstrap_stack_bottom;
    Context.BootstrapStackEnd = (uintptr_t)__bootstrap_stack_top;
    Context.AvailableMemoryBytes = 0;
    Context.MemorySizeAvailable = false;
    Initialized = false;
    if (BootMagic != OESDK_MULTIBOOT1_BOOT_MAGIC || BootInformationAddress == 0U) return false;
    Context.BootProtocol = OesdkBootProtocolMultiboot1;
    Context.MemorySizeAvailable = ReadAvailableMemory((const OesdkMultibootInformation *)BootInformationAddress, &Context.AvailableMemoryBytes);
    Initialized = true;
    return true;
}

const OesdkBootContext *OesdkBootContextGet(void) { return Initialized ? &Context : NULL; }
