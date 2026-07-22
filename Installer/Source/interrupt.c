#include <oesdk/interrupt.h>
#include <oesdk/gdt.h>
#include <oesdk/panic.h>
#include <stddef.h>
#include <stdint.h>

typedef struct __attribute__((packed)) OesdkIdtGate {
    uint16_t OffsetLow;
    uint16_t Selector;
    uint8_t Ist;
    uint8_t Attributes;
    uint16_t OffsetMiddle;
    uint32_t OffsetHigh;
    uint32_t Reserved;
} OesdkIdtGate;

typedef struct __attribute__((packed)) OesdkIdtPointer {
    uint16_t Limit;
    uint64_t Base;
} OesdkIdtPointer;

_Static_assert(sizeof(OesdkIdtGate) == 16U, "IDT gates must be 16 bytes");

extern void (*OesdkExceptionStubTable[OESDK_EXCEPTION_VECTOR_COUNT])(void);
static OesdkIdtGate OesdkIdt[OESDK_IDT_VECTOR_COUNT] __attribute__((aligned(16)));
static OesdkIdtInformation OesdkInformation;

static const char *const OesdkExceptionNames[OESDK_EXCEPTION_VECTOR_COUNT] = {
    "Divide Error", "Debug", "Non-Maskable Interrupt", "Breakpoint",
    "Overflow", "BOUND Range Exceeded", "Invalid Opcode", "Device Not Available",
    "Double Fault", "Coprocessor Segment Overrun", "Invalid TSS", "Segment Not Present",
    "Stack-Segment Fault", "General Protection Fault", "Page Fault", "Reserved",
    "x87 Floating-Point Exception", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception",
    "Virtualization Exception", "Control Protection Exception", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Hypervisor Injection Exception", "VMM Communication Exception", "Security Exception", "Reserved"
};

static void OesdkZero(void *Address, size_t Size) {
    uint8_t *Bytes = (uint8_t *)Address;
    for (size_t Index = 0; Index < Size; ++Index) Bytes[Index] = 0U;
}

static void OesdkSetGate(uint8_t Vector, uintptr_t Handler, uint8_t Ist) {
    OesdkIdtGate *Gate = &OesdkIdt[Vector];
    Gate->OffsetLow = (uint16_t)(Handler & UINT64_C(0xFFFF));
    Gate->Selector = OESDK_GDT_KERNEL_CODE_SELECTOR;
    Gate->Ist = (uint8_t)(Ist & 0x07U);
    Gate->Attributes = 0x8EU;
    Gate->OffsetMiddle = (uint16_t)((Handler >> 16U) & UINT64_C(0xFFFF));
    Gate->OffsetHigh = (uint32_t)((Handler >> 32U) & UINT64_C(0xFFFFFFFF));
    Gate->Reserved = 0U;
}

const char *OesdkExceptionName(uint8_t Vector) {
    return Vector < OESDK_EXCEPTION_VECTOR_COUNT ? OesdkExceptionNames[Vector] : "Unknown Interrupt";
}

__attribute__((noreturn)) void OesdkInterruptDispatch(OesdkInterruptFrame *Frame) {
    uint64_t Code = UINT64_C(0x17010000) | (Frame->Vector & UINT64_C(0xFF));
    OesdkPanicAt("Exception", OesdkExceptionName((uint8_t)Frame->Vector), Code,
                 "<interrupt>", "OesdkInterruptDispatch", (uint32_t)Frame->Vector);
}

bool OesdkIdtInitialize(void) {
    OesdkZero(OesdkIdt, sizeof(OesdkIdt));
    for (uint8_t Vector = 0U; Vector < OESDK_EXCEPTION_VECTOR_COUNT; ++Vector) {
        uint8_t Ist = 0U;
        if (Vector == 8U) Ist = 1U;
        else if (Vector == 2U) Ist = 2U;
        else if (Vector == 18U) Ist = 3U;
        OesdkSetGate(Vector, (uintptr_t)OesdkExceptionStubTable[Vector], Ist);
    }
    OesdkIdtPointer Pointer = { (uint16_t)(sizeof(OesdkIdt) - 1U), (uintptr_t)OesdkIdt };
    __asm__ volatile("lidt %0" : : "m"(Pointer) : "memory");
    OesdkInformation.Base = Pointer.Base;
    OesdkInformation.Limit = Pointer.Limit;
    OesdkInformation.Initialized = true;
    return true;
}

const OesdkIdtInformation *OesdkIdtInformationGet(void) {
    return OesdkInformation.Initialized ? &OesdkInformation : NULL;
}
