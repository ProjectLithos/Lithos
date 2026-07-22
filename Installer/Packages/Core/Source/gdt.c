#include <oesdk/gdt.h>
#include <oesdk/cpu.h>
#include <stddef.h>
#include <stdint.h>

#define OESDK_GDT_TSS_LOW_INDEX  5U
#define OESDK_GDT_TSS_HIGH_INDEX 6U

typedef struct __attribute__((packed)) OesdkGdtPointer {
    uint16_t Limit;
    uint64_t Base;
} OesdkGdtPointer;

typedef struct __attribute__((packed)) OesdkTaskStateSegment {
    uint32_t Reserved0;
    uint64_t Rsp0;
    uint64_t Rsp1;
    uint64_t Rsp2;
    uint64_t Reserved1;
    uint64_t Ist[OESDK_GDT_IST_ENTRY_COUNT];
    uint64_t Reserved2;
    uint16_t Reserved3;
    uint16_t IoMapBase;
} OesdkTaskStateSegment;

_Static_assert(sizeof(OesdkTaskStateSegment) == 104U, "The x86-64 TSS must be 104 bytes");
_Static_assert(OESDK_GDT_TSS_SELECTOR == (OESDK_GDT_TSS_LOW_INDEX * 8U), "TSS selector/index mismatch");

static uint64_t OesdkGdt[OESDK_GDT_ENTRY_COUNT] __attribute__((aligned(16)));
static OesdkTaskStateSegment OesdkTss __attribute__((aligned(16)));
static uint8_t OesdkIstStacks[OESDK_GDT_IST_ENTRY_COUNT][OESDK_GDT_IST_STACK_SIZE] __attribute__((aligned(16)));
static OesdkGdtInformation OesdkInformation;
static bool OesdkInitialized;

static void OesdkMemoryZero(void *Address, size_t Size) {
    uint8_t *Bytes = (uint8_t *)Address;
    for (size_t Index = 0; Index < Size; ++Index) Bytes[Index] = 0U;
}

static bool OesdkStackTopIsValid(uintptr_t StackTop) {
    return StackTop != 0U && (StackTop & 0x0FU) == 0U;
}

static void OesdkSetTssDescriptor(uintptr_t Base, uint32_t Limit) {
    uint64_t Low = 0U;
    Low |= (uint64_t)(Limit & 0xFFFFU);
    Low |= (uint64_t)(Base & 0xFFFFFFU) << 16U;
    Low |= UINT64_C(0x89) << 40U;
    Low |= (uint64_t)((Limit >> 16U) & 0x0FU) << 48U;
    Low |= (uint64_t)((Base >> 24U) & 0xFFU) << 56U;
    OesdkGdt[OESDK_GDT_TSS_LOW_INDEX] = Low;
    OesdkGdt[OESDK_GDT_TSS_HIGH_INDEX] = (uint64_t)(Base >> 32U);
}

static void OesdkLoadGdt(const OesdkGdtPointer *Pointer) {
    __asm__ volatile(
        "lgdt (%0)\n"
        "pushq $0x08\n"
        "leaq 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        "movw $0x10, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%ss\n"
        "xorw %%ax, %%ax\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        : : "r"(Pointer) : "rax", "memory");
}

static void OesdkLoadTaskRegister(void) {
    uint16_t Selector = OESDK_GDT_TSS_SELECTOR;
    __asm__ volatile("ltr %0" : : "r"(Selector) : "memory");
}

bool OesdkGdtInitialize(void) {
    if (OesdkInitialized) return true;

    uint64_t Flags = OesdkCpuInterruptStateSave();
    OesdkMemoryZero(OesdkGdt, sizeof(OesdkGdt));
    OesdkMemoryZero(&OesdkTss, sizeof(OesdkTss));
    OesdkMemoryZero(&OesdkInformation, sizeof(OesdkInformation));

    OesdkGdt[0] = UINT64_C(0x0000000000000000);
    OesdkGdt[1] = UINT64_C(0x00AF9A000000FFFF);
    OesdkGdt[2] = UINT64_C(0x00AF92000000FFFF);
    OesdkGdt[3] = UINT64_C(0x00AFF2000000FFFF);
    OesdkGdt[4] = UINT64_C(0x00AFFA000000FFFF);

    for (size_t Index = 0U; Index < OESDK_GDT_IST_ENTRY_COUNT; ++Index) {
        uintptr_t StackTop = (uintptr_t)&OesdkIstStacks[Index][OESDK_GDT_IST_STACK_SIZE];
        OesdkTss.Ist[Index] = StackTop;
        OesdkInformation.InterruptStackTable[Index] = StackTop;
    }

    OesdkTss.IoMapBase = (uint16_t)sizeof(OesdkTaskStateSegment);
    OesdkSetTssDescriptor((uintptr_t)&OesdkTss, (uint32_t)(sizeof(OesdkTss) - 1U));

    OesdkGdtPointer Pointer = {
        .Limit = (uint16_t)(sizeof(OesdkGdt) - 1U),
        .Base = (uintptr_t)OesdkGdt
    };

    OesdkLoadGdt(&Pointer);
    OesdkLoadTaskRegister();

    OesdkInformation.Limit = Pointer.Limit;
    OesdkInformation.Base = (uintptr_t)OesdkGdt;
    OesdkInformation.TaskStateSelector = OESDK_GDT_TSS_SELECTOR;
    OesdkInformation.Ring0Stack = OesdkTss.Rsp0;
    OesdkInformation.DescriptorCount = OESDK_GDT_ENTRY_COUNT;
    OesdkInformation.TaskRegisterLoaded = true;
    OesdkInformation.Initialized = true;
    OesdkInitialized = true;

    OesdkCpuInterruptStateRestore(Flags);
    return true;
}

bool OesdkGdtIsInitialized(void) {
    return OesdkInitialized;
}

const OesdkGdtInformation *OesdkGdtInformationGet(void) {
    return OesdkInitialized ? &OesdkInformation : NULL;
}

bool OesdkGdtSetKernelStack(uintptr_t StackTop) {
    if (!OesdkInitialized || !OesdkStackTopIsValid(StackTop)) return false;
    OesdkTss.Rsp0 = StackTop;
    OesdkInformation.Ring0Stack = StackTop;
    return true;
}

bool OesdkGdtSetInterruptStack(uint8_t IstIndex, uintptr_t StackTop) {
    if (!OesdkInitialized || IstIndex == 0U || IstIndex > OESDK_GDT_IST_ENTRY_COUNT || !OesdkStackTopIsValid(StackTop)) {
        return false;
    }
    OesdkTss.Ist[IstIndex - 1U] = StackTop;
    OesdkInformation.InterruptStackTable[IstIndex - 1U] = StackTop;
    return true;
}
