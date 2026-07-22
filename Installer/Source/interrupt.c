#include "internal.h"
#include <oesdk/cpu.h>
#include <oesdk/console.h>
#include <oesdk/gdt.h>
#include <oesdk/interrupt.h>
#include <oesdk/panic.h>
#include <stddef.h>
#include <stdint.h>

#define OESDK_IDT_INTERRUPT_GATE 0x8EU
#define OESDK_EXCEPTION_PANIC_BASE UINT64_C(0x17010000)

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

typedef struct OesdkHandlerSlot {
    OesdkInterruptHandler Handler;
    void *Context;
} OesdkHandlerSlot;

_Static_assert(sizeof(OesdkIdtGate) == 16U, "IDT gates must be 16 bytes");
_Static_assert(__builtin_offsetof(OesdkInterruptFrame, Vector) == 120U, "interrupt frame register layout changed");
_Static_assert(__builtin_offsetof(OesdkInterruptFrame, Rip) == 136U, "interrupt frame CPU layout changed");
_Static_assert(__builtin_offsetof(OesdkInterruptFrame, Rsp) == 160U, "interrupt frame stack layout changed");
_Static_assert(sizeof(OesdkInterruptFrame) == 176U, "interrupt frame size changed");

extern void (*OesdkInterruptStubTable[OESDK_IDT_VECTOR_COUNT])(void);
static OesdkIdtGate OesdkIdt[OESDK_IDT_VECTOR_COUNT] __attribute__((aligned(16)));
static OesdkHandlerSlot OesdkHandlers[OESDK_IDT_VECTOR_COUNT];
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

static void OesdkSetGate(uint16_t Vector, uintptr_t Handler, uint8_t Ist) {
    OesdkIdtGate *Gate = &OesdkIdt[Vector];
    Gate->OffsetLow = (uint16_t)(Handler & UINT64_C(0xFFFF));
    Gate->Selector = OESDK_GDT_KERNEL_CODE_SELECTOR;
    Gate->Ist = (uint8_t)(Ist & 0x07U);
    Gate->Attributes = OESDK_IDT_INTERRUPT_GATE;
    Gate->OffsetMiddle = (uint16_t)((Handler >> 16U) & UINT64_C(0xFFFF));
    Gate->OffsetHigh = (uint32_t)((Handler >> 32U) & UINT64_C(0xFFFFFFFF));
    Gate->Reserved = 0U;
}

static void OesdkPrintRegister(const char *Name, uint64_t Value) {
    oesdk_serial_printf("%s=0x%016llX ", Name, Value);
    kprintf("%s=0x%016llX ", Name, Value);
}

const char *OesdkExceptionName(uint8_t Vector) {
    return Vector < OESDK_EXCEPTION_VECTOR_COUNT ? OesdkExceptionNames[Vector] : "Hardware or Software Interrupt";
}

OesdkPageFaultInformation OesdkPageFaultDecode(uintptr_t Address, uint64_t ErrorCode) {
    OesdkPageFaultInformation Information;
    Information.Address = Address;
    Information.ErrorCode = ErrorCode;
    Information.PresentViolation = (ErrorCode & (UINT64_C(1) << 0U)) != 0U;
    Information.WriteAccess = (ErrorCode & (UINT64_C(1) << 1U)) != 0U;
    Information.UserAccess = (ErrorCode & (UINT64_C(1) << 2U)) != 0U;
    Information.ReservedBitViolation = (ErrorCode & (UINT64_C(1) << 3U)) != 0U;
    Information.InstructionFetch = (ErrorCode & (UINT64_C(1) << 4U)) != 0U;
    Information.ProtectionKeyViolation = (ErrorCode & (UINT64_C(1) << 5U)) != 0U;
    Information.ShadowStackAccess = (ErrorCode & (UINT64_C(1) << 6U)) != 0U;
    Information.SgxViolation = (ErrorCode & (UINT64_C(1) << 15U)) != 0U;
    return Information;
}

void OesdkInterruptFrameDump(const OesdkInterruptFrame *Frame) {
    if (Frame == NULL) return;
    oesdk_serial_printf("\n[FAIL] Interrupt frame\n");
    kprintf("\n[FAIL] Interrupt frame\n");
    OesdkPrintRegister("RIP", Frame->Rip); OesdkPrintRegister("CS", Frame->Cs); OesdkPrintRegister("RFLAGS", Frame->Rflags);
    if ((Frame->Cs & UINT64_C(3)) != 0U || Frame->Vector == 2U || Frame->Vector == 8U || Frame->Vector == 18U) {
        OesdkPrintRegister("RSP", Frame->Rsp); OesdkPrintRegister("SS", Frame->Ss);
    }
    oesdk_serial_write("\n"); kputc('\n');
    OesdkPrintRegister("RAX", Frame->Rax); OesdkPrintRegister("RBX", Frame->Rbx); OesdkPrintRegister("RCX", Frame->Rcx); OesdkPrintRegister("RDX", Frame->Rdx);
    oesdk_serial_write("\n"); kputc('\n');
    OesdkPrintRegister("RSI", Frame->Rsi); OesdkPrintRegister("RDI", Frame->Rdi); OesdkPrintRegister("RBP", Frame->Rbp);
    oesdk_serial_write("\n"); kputc('\n');
    OesdkPrintRegister("R8", Frame->R8); OesdkPrintRegister("R9", Frame->R9); OesdkPrintRegister("R10", Frame->R10); OesdkPrintRegister("R11", Frame->R11);
    oesdk_serial_write("\n"); kputc('\n');
    OesdkPrintRegister("R12", Frame->R12); OesdkPrintRegister("R13", Frame->R13); OesdkPrintRegister("R14", Frame->R14); OesdkPrintRegister("R15", Frame->R15);
    oesdk_serial_write("\n"); kputc('\n');
    OesdkPrintRegister("VECTOR", Frame->Vector); OesdkPrintRegister("ERROR", Frame->ErrorCode);
    oesdk_serial_write("\n"); kputc('\n');
}

static void OesdkFatalException(OesdkInterruptFrame *Frame, const char *Reason) {
    if (Frame == NULL) {
        OesdkPanic("Exception", "Missing interrupt frame", OESDK_EXCEPTION_PANIC_BASE);
    }
    oesdk_serial_printf("\n[FAIL] CPU exception %llu: %s\n", Frame->Vector, Reason);
    kprintf("\n[FAIL] CPU exception %llu: %s\n", Frame->Vector, Reason);
    OesdkInterruptFrameDump(Frame);
    OesdkPanic("Exception", Reason, OESDK_EXCEPTION_PANIC_BASE | (Frame->Vector & UINT64_C(0xFF)));
}

void OesdkDivideErrorHandler(OesdkInterruptFrame *Frame) {
    OesdkFatalException(Frame, "Divide Error");
}

void OesdkInvalidOpcodeHandler(OesdkInterruptFrame *Frame) {
    OesdkFatalException(Frame, "Invalid Opcode");
}

void OesdkGeneralProtectionFaultHandler(OesdkInterruptFrame *Frame) {
    OesdkFatalException(Frame, "General Protection Fault");
}

void OesdkPageFaultHandler(OesdkInterruptFrame *Frame) {
    if (Frame == NULL) {
        OesdkPanic("Exception", "Missing page-fault frame", OESDK_EXCEPTION_PANIC_BASE | UINT64_C(14));
    }
    OesdkPageFaultInformation Fault = OesdkPageFaultDecode(OesdkCpuReadCr2(), Frame->ErrorCode);
    oesdk_serial_printf("\n[FAIL] CPU exception 14: Page Fault\n");
    oesdk_serial_printf("[FAIL] Page fault address: 0x%016llX\n", (uint64_t)Fault.Address);
    oesdk_serial_printf("[FAIL] Cause: %s, %s, %s%s%s%s%s%s\n",
        Fault.PresentViolation ? "protection violation" : "non-present page",
        Fault.WriteAccess ? "write" : "read",
        Fault.UserAccess ? "user" : "supervisor",
        Fault.ReservedBitViolation ? ", reserved-bit" : "",
        Fault.InstructionFetch ? ", instruction-fetch" : "",
        Fault.ProtectionKeyViolation ? ", protection-key" : "",
        Fault.ShadowStackAccess ? ", shadow-stack" : "",
        Fault.SgxViolation ? ", SGX" : "");
    kprintf("\n[FAIL] CPU exception 14: Page Fault\n");
    kprintf("[FAIL] Page fault address: 0x%016llX\n", (uint64_t)Fault.Address);
    kprintf("[FAIL] Cause: %s, %s, %s%s%s%s%s%s\n",
        Fault.PresentViolation ? "protection violation" : "non-present page",
        Fault.WriteAccess ? "write" : "read",
        Fault.UserAccess ? "user" : "supervisor",
        Fault.ReservedBitViolation ? ", reserved-bit" : "",
        Fault.InstructionFetch ? ", instruction-fetch" : "",
        Fault.ProtectionKeyViolation ? ", protection-key" : "",
        Fault.ShadowStackAccess ? ", shadow-stack" : "",
        Fault.SgxViolation ? ", SGX" : "");
    OesdkInterruptFrameDump(Frame);
    OesdkPanic("Exception", "Page Fault", OESDK_EXCEPTION_PANIC_BASE | UINT64_C(14));
}

void OesdkDoubleFaultHandler(OesdkInterruptFrame *Frame) {
    OesdkFatalException(Frame, "Double Fault");
}

void OesdkMachineCheckHandler(OesdkInterruptFrame *Frame) {
    OesdkFatalException(Frame, "Machine Check");
}

void OesdkInterruptDispatch(OesdkInterruptFrame *Frame) {
    if (Frame == NULL || Frame->Vector >= OESDK_IDT_VECTOR_COUNT) {
        OesdkPanic("Interrupts", "Invalid interrupt frame", OESDK_EXCEPTION_PANIC_BASE);
    }

    OesdkHandlerSlot Slot = OesdkHandlers[Frame->Vector];
    if (Slot.Handler != NULL) {
        Slot.Handler(Frame, Slot.Context);
        return;
    }

    if (Frame->Vector >= OESDK_EXCEPTION_VECTOR_COUNT) return;

    switch (Frame->Vector) {
        case 0U: OesdkDivideErrorHandler(Frame); break;
        case 6U: OesdkInvalidOpcodeHandler(Frame); break;
        case 8U: OesdkDoubleFaultHandler(Frame); break;
        case 13U: OesdkGeneralProtectionFaultHandler(Frame); break;
        case 14U: OesdkPageFaultHandler(Frame); break;
        case 18U: OesdkMachineCheckHandler(Frame); break;
        default: OesdkFatalException(Frame, OesdkExceptionName((uint8_t)Frame->Vector)); break;
    }
}

OesdkStatus OesdkIdtInitialize(void) {
    const OesdkGdtInformation *Gdt = OesdkGdtInformationGet();
    if (Gdt == NULL) return OESDK_STATUS_NOT_FOUND;

    uint64_t Flags = OesdkCpuInterruptStateSave();
    OesdkZero(OesdkIdt, sizeof(OesdkIdt));
    OesdkZero(OesdkHandlers, sizeof(OesdkHandlers));
    OesdkZero(&OesdkInformation, sizeof(OesdkInformation));

    for (uint16_t Vector = 0U; Vector < OESDK_IDT_VECTOR_COUNT; ++Vector) {
        uint8_t Ist = 0U;
        if (Vector == 8U) Ist = OESDK_GDT_IST_DOUBLE_FAULT;
        else if (Vector == 2U) Ist = OESDK_GDT_IST_NMI;
        else if (Vector == 18U) Ist = OESDK_GDT_IST_MACHINE_CHECK;
        OesdkSetGate(Vector, (uintptr_t)OesdkInterruptStubTable[Vector], Ist);
    }

    OesdkIdtPointer Pointer = { (uint16_t)(sizeof(OesdkIdt) - 1U), (uintptr_t)OesdkIdt };
    __asm__ volatile("lidt %0" : : "m"(Pointer) : "memory");
    OesdkInformation.Base = Pointer.Base;
    OesdkInformation.Limit = Pointer.Limit;
    OesdkInformation.Initialized = true;
    OesdkInformation.InstalledVectors = OESDK_IDT_VECTOR_COUNT;
    OesdkCpuInterruptStateRestore(Flags);
    return OESDK_STATUS_SUCCESS;
}

const OesdkIdtInformation *OesdkIdtInformationGet(void) {
    return OesdkInformation.Initialized ? &OesdkInformation : NULL;
}

OesdkStatus OesdkInterruptHandlerRegister(uint8_t Vector, OesdkInterruptHandler Handler, void *Context) {
    if (!OesdkInformation.Initialized) return OESDK_STATUS_NOT_FOUND;
    if (Handler == NULL) return OESDK_STATUS_INVALID_ARGUMENT;
    uint64_t Flags = OesdkCpuInterruptStateSave();
    if (OesdkHandlers[Vector].Handler != NULL) {
        OesdkCpuInterruptStateRestore(Flags);
        return OESDK_STATUS_ALREADY_EXISTS;
    }
    OesdkHandlers[Vector].Context = Context;
    OesdkHandlers[Vector].Handler = Handler;
    OesdkCpuInterruptStateRestore(Flags);
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkInterruptHandlerUnregister(uint8_t Vector, OesdkInterruptHandler Handler) {
    if (!OesdkInformation.Initialized) return OESDK_STATUS_NOT_FOUND;
    if (Handler == NULL) return OESDK_STATUS_INVALID_ARGUMENT;
    uint64_t Flags = OesdkCpuInterruptStateSave();
    if (OesdkHandlers[Vector].Handler != Handler) {
        OesdkCpuInterruptStateRestore(Flags);
        return OESDK_STATUS_NOT_FOUND;
    }
    OesdkHandlers[Vector].Handler = NULL;
    OesdkHandlers[Vector].Context = NULL;
    OesdkCpuInterruptStateRestore(Flags);
    return OESDK_STATUS_SUCCESS;
}

bool OesdkInterruptHandlerIsRegistered(uint8_t Vector) {
    return OesdkHandlers[Vector].Handler != NULL;
}
