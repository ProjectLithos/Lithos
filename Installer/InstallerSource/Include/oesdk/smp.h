#ifndef OESDK_SMP_H
#define OESDK_SMP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_SMP_MAXIMUM_CPUS 256U
#define OESDK_SMP_TRAMPOLINE_ALIGNMENT 4096U
#define OESDK_SMP_TRAMPOLINE_MAXIMUM_ADDRESS UINT64_C(0x00000000000FF000)

typedef enum OesdkCpuStartupState
{
    OesdkCpuStartupAbsent = 0,
    OesdkCpuStartupDiscovered,
    OesdkCpuStartupPrepared,
    OesdkCpuStartupStarting,
    OesdkCpuStartupOnline,
    OesdkCpuStartupFailed
} OesdkCpuStartupState;

typedef struct OesdkSmpProcessor
{
    uint32_t LogicalId;
    uint32_t ApicId;
    uintptr_t KernelStackTop;
    uintptr_t PerCpuPointer;
    volatile uint32_t State;
    OesdkStatus StartupStatus;
    bool BootstrapProcessor;
    bool Enabled;
} OesdkSmpProcessor;

typedef void (*OesdkSmpDelayMicroseconds)(uint32_t Microseconds, void *Context);
typedef OesdkStatus (*OesdkSmpPrepareTrampoline)(uintptr_t TrampolineAddress,
                                                 uintptr_t PageTableRoot,
                                                 uintptr_t EntryPoint,
                                                 const OesdkSmpProcessor *Processor,
                                                 void *Context);
typedef void (*OesdkSmpApplicationProcessorEntry)(uint32_t LogicalId, void *Context);

typedef struct OesdkSmpConfiguration
{
    uintptr_t TrampolineAddress;
    uintptr_t PageTableRoot;
    uintptr_t ApplicationProcessorEntryPoint;
    uint32_t StartupTimeoutMicroseconds;
    OesdkSmpDelayMicroseconds DelayMicroseconds;
    OesdkSmpPrepareTrampoline PrepareTrampoline;
    OesdkSmpApplicationProcessorEntry ApplicationProcessorEntry;
    void *Context;
} OesdkSmpConfiguration;

typedef struct OesdkSmpInformation
{
    uint32_t DiscoveredProcessorCount;
    uint32_t EnabledProcessorCount;
    uint32_t OnlineProcessorCount;
    uint32_t BootstrapLogicalId;
    bool Initialized;
    bool StartupComplete;
} OesdkSmpInformation;

OesdkStatus OesdkSmpInitialize(const OesdkSmpConfiguration *Configuration,
                               const uint32_t *ApicIds,
                               const bool *Enabled,
                               uint32_t ProcessorCount,
                               uint32_t BootstrapApicId);
OesdkStatus OesdkSmpSetProcessorResources(uint32_t LogicalId, uintptr_t KernelStackTop, uintptr_t PerCpuPointer);
OesdkStatus OesdkSmpStartApplicationProcessors(void);
void OesdkSmpApplicationProcessorOnline(uint32_t ApicId);
void OesdkSmpApplicationProcessorMain(uint32_t ApicId);
const OesdkSmpProcessor *OesdkSmpProcessorGet(uint32_t LogicalId);
const OesdkSmpProcessor *OesdkSmpCurrentProcessor(void);
const OesdkSmpInformation *OesdkSmpInformationGet(void);
uint32_t OesdkSmpCurrentApicId(void);
bool OesdkSmpIsBootstrapProcessor(void);

#ifdef __cplusplus
}
#endif

#endif
