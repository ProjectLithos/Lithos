#ifndef OESDK_CPU_H
#define OESDK_CPU_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OesdkCpuRegisters
{
    uint32_t Eax;
    uint32_t Ebx;
    uint32_t Ecx;
    uint32_t Edx;
} OesdkCpuRegisters;

typedef struct OesdkCpuFeatures
{
    char Vendor[13];
    uint32_t MaximumBasicLeaf;
    uint32_t MaximumExtendedLeaf;
    bool LongMode;
    bool Nx;
    bool Apic;
    bool X2Apic;
    bool Syscall;
    bool Tsc;
    bool InvariantTsc;
    bool Page1GiB;
} OesdkCpuFeatures;

bool OesdkCpuInitialize(void);
const OesdkCpuFeatures *OesdkCpuFeaturesGet(void);
void OesdkCpuQuery(uint32_t Leaf, uint32_t Subleaf, OesdkCpuRegisters *Registers);

void OesdkCpuDisableInterrupts(void);
void OesdkCpuEnableInterrupts(void);
uint64_t OesdkCpuInterruptStateSave(void);
void OesdkCpuInterruptStateRestore(uint64_t Flags);
void OesdkCpuHalt(void);
void OesdkCpuPause(void);

uint64_t OesdkCpuReadFlags(void);
uint64_t OesdkCpuReadCr0(void);
uint64_t OesdkCpuReadCr2(void);
uint64_t OesdkCpuReadCr3(void);
uint64_t OesdkCpuReadCr4(void);
void OesdkCpuWriteCr3(uint64_t Value);
void OesdkCpuInvalidatePage(uintptr_t VirtualAddress);

#ifdef __cplusplus
}
#endif

#endif
