#ifndef OESDK_INTERRUPT_H
#define OESDK_INTERRUPT_H

#include <stdbool.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_IDT_VECTOR_COUNT 256U
#define OESDK_EXCEPTION_VECTOR_COUNT 32U
#define OESDK_INTERRUPT_VECTOR_FIRST 32U
#define OESDK_INTERRUPT_VECTOR_LAST 255U

#define OESDK_PAGE_FAULT_PRESENT_BIT          (UINT64_C(1) << 0U)
#define OESDK_PAGE_FAULT_WRITE_BIT            (UINT64_C(1) << 1U)
#define OESDK_PAGE_FAULT_USER_BIT             (UINT64_C(1) << 2U)
#define OESDK_PAGE_FAULT_RESERVED_BIT         (UINT64_C(1) << 3U)
#define OESDK_PAGE_FAULT_INSTRUCTION_BIT      (UINT64_C(1) << 4U)
#define OESDK_PAGE_FAULT_PROTECTION_KEY_BIT   (UINT64_C(1) << 5U)
#define OESDK_PAGE_FAULT_SHADOW_STACK_BIT     (UINT64_C(1) << 6U)
#define OESDK_PAGE_FAULT_SGX_BIT              (UINT64_C(1) << 15U)

typedef struct OesdkInterruptFrame {
    uint64_t R15;
    uint64_t R14;
    uint64_t R13;
    uint64_t R12;
    uint64_t R11;
    uint64_t R10;
    uint64_t R9;
    uint64_t R8;
    uint64_t Rdi;
    uint64_t Rsi;
    uint64_t Rbp;
    uint64_t Rdx;
    uint64_t Rcx;
    uint64_t Rbx;
    uint64_t Rax;
    uint64_t Vector;
    uint64_t ErrorCode;
    uint64_t Rip;
    uint64_t Cs;
    uint64_t Rflags;
    uint64_t Rsp;
    uint64_t Ss;
} OesdkInterruptFrame;

typedef struct OesdkPageFaultInformation {
    uintptr_t Address;
    uint64_t ErrorCode;
    bool PresentViolation;
    bool WriteAccess;
    bool UserAccess;
    bool ReservedBitViolation;
    bool InstructionFetch;
    bool ProtectionKeyViolation;
    bool ShadowStackAccess;
    bool SgxViolation;
} OesdkPageFaultInformation;

typedef struct OesdkIdtInformation {
    uintptr_t Base;
    uint16_t Limit;
    bool Initialized;
    uint16_t InstalledVectors;
} OesdkIdtInformation;

typedef void (*OesdkInterruptHandler)(OesdkInterruptFrame *Frame, void *Context);

OesdkStatus OesdkIdtInitialize(void);
const OesdkIdtInformation *OesdkIdtInformationGet(void);
const char *OesdkExceptionName(uint8_t Vector);
OesdkStatus OesdkInterruptHandlerRegister(uint8_t Vector, OesdkInterruptHandler Handler, void *Context);
OesdkStatus OesdkInterruptHandlerUnregister(uint8_t Vector, OesdkInterruptHandler Handler);
bool OesdkInterruptHandlerIsRegistered(uint8_t Vector);
OesdkPageFaultInformation OesdkPageFaultDecode(uintptr_t Address, uint64_t ErrorCode);
OesdkPageFaultInformation OesdkPageFaultDecodeCurrent(uint64_t ErrorCode);
void OesdkInterruptFrameDump(const OesdkInterruptFrame *Frame);

void OesdkDivideErrorHandler(OesdkInterruptFrame *Frame);
void OesdkInvalidOpcodeHandler(OesdkInterruptFrame *Frame);
void OesdkGeneralProtectionFaultHandler(OesdkInterruptFrame *Frame);
void OesdkPageFaultHandler(OesdkInterruptFrame *Frame);
void OesdkDoubleFaultHandler(OesdkInterruptFrame *Frame);
void OesdkMachineCheckHandler(OesdkInterruptFrame *Frame);

#ifdef __cplusplus
}
#endif

#endif
