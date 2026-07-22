#ifndef OESDK_INTERRUPT_H
#define OESDK_INTERRUPT_H

#include <stdbool.h>
#include <stdint.h>

#define OESDK_IDT_VECTOR_COUNT 256U
#define OESDK_EXCEPTION_VECTOR_COUNT 32U

typedef struct OesdkInterruptFrame {
    uint64_t R15, R14, R13, R12, R11, R10, R9, R8;
    uint64_t Rdi, Rsi, Rbp, Rdx, Rcx, Rbx, Rax;
    uint64_t Vector;
    uint64_t ErrorCode;
    uint64_t Rip;
    uint64_t Cs;
    uint64_t Rflags;
} OesdkInterruptFrame;

typedef struct OesdkIdtInformation {
    uintptr_t Base;
    uint16_t Limit;
    bool Initialized;
} OesdkIdtInformation;

bool OesdkIdtInitialize(void);
const OesdkIdtInformation *OesdkIdtInformationGet(void);
const char *OesdkExceptionName(uint8_t Vector);

#endif
