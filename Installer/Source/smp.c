#include <oesdk/smp.h>
#include <oesdk/cpu.h>
#include <oesdk/interrupt_controller.h>
#include <stddef.h>

#define OESDK_SMP_DEFAULT_TIMEOUT_US 100000U
#define OESDK_SMP_INIT_DELAY_US 10000U
#define OESDK_SMP_SIPI_DELAY_US 200U
#define OESDK_SMP_POLL_DELAY_US 10U

static OesdkSmpProcessor Processors[OESDK_SMP_MAXIMUM_CPUS];
static OesdkSmpInformation Information;
static OesdkSmpConfiguration Configuration;

static OesdkSmpProcessor *FindByApicId(uint32_t ApicId)
{
    uint32_t Index;
    for (Index = 0U; Index < Information.DiscoveredProcessorCount; ++Index) {
        if (Processors[Index].ApicId == ApicId) return &Processors[Index];
    }
    return NULL;
}

static void Delay(uint32_t Microseconds)
{
    if (Configuration.DelayMicroseconds != NULL) {
        Configuration.DelayMicroseconds(Microseconds, Configuration.Context);
    } else {
        uint32_t Outer;
        for (Outer = 0U; Outer < Microseconds; ++Outer) {
            uint32_t Inner;
            for (Inner = 0U; Inner < 64U; ++Inner) OesdkCpuPause();
        }
    }
}

OesdkStatus OesdkSmpInitialize(const OesdkSmpConfiguration *NewConfiguration,
                               const uint32_t *ApicIds,
                               const bool *Enabled,
                               uint32_t ProcessorCount,
                               uint32_t BootstrapApicId)
{
    uint32_t Index;
    bool BootstrapFound = false;
    if (NewConfiguration == NULL || ApicIds == NULL || ProcessorCount == 0U ||
        ProcessorCount > OESDK_SMP_MAXIMUM_CPUS) return OESDK_STATUS_INVALID_ARGUMENT;
    if (NewConfiguration->TrampolineAddress == 0U ||
        (NewConfiguration->TrampolineAddress & (OESDK_SMP_TRAMPOLINE_ALIGNMENT - 1U)) != 0U ||
        NewConfiguration->TrampolineAddress > OESDK_SMP_TRAMPOLINE_MAXIMUM_ADDRESS ||
        NewConfiguration->PageTableRoot == 0U || NewConfiguration->ApplicationProcessorEntryPoint == 0U ||
        NewConfiguration->PrepareTrampoline == NULL) return OESDK_STATUS_INVALID_ARGUMENT;
    if (OesdkInterruptControllerInformationGet() == NULL) return OESDK_STATUS_NOT_FOUND;

    Configuration = *NewConfiguration;
    if (Configuration.StartupTimeoutMicroseconds == 0U) Configuration.StartupTimeoutMicroseconds = OESDK_SMP_DEFAULT_TIMEOUT_US;
    Information.DiscoveredProcessorCount = ProcessorCount;
    Information.EnabledProcessorCount = 0U;
    Information.OnlineProcessorCount = 0U;
    Information.BootstrapLogicalId = 0U;
    Information.StartupComplete = false;

    for (Index = 0U; Index < ProcessorCount; ++Index) {
        uint32_t Previous;
        Processors[Index].LogicalId = Index;
        Processors[Index].ApicId = ApicIds[Index];
        Processors[Index].KernelStackTop = 0U;
        Processors[Index].PerCpuPointer = 0U;
        Processors[Index].StartupStatus = OESDK_STATUS_SUCCESS;
        Processors[Index].BootstrapProcessor = ApicIds[Index] == BootstrapApicId;
        Processors[Index].Enabled = Enabled == NULL || Enabled[Index];
        Processors[Index].State = Processors[Index].Enabled ? OesdkCpuStartupDiscovered : OesdkCpuStartupAbsent;
        if (Processors[Index].Enabled) ++Information.EnabledProcessorCount;
        if (Processors[Index].BootstrapProcessor) {
            if (!Processors[Index].Enabled || BootstrapFound) return OESDK_STATUS_CORRUPT;
            BootstrapFound = true;
            Information.BootstrapLogicalId = Index;
            Processors[Index].State = OesdkCpuStartupOnline;
            Information.OnlineProcessorCount = 1U;
        }
        for (Previous = 0U; Previous < Index; ++Previous) {
            if (Processors[Previous].ApicId == ApicIds[Index]) return OESDK_STATUS_ALREADY_EXISTS;
        }
    }
    if (!BootstrapFound) return OESDK_STATUS_NOT_FOUND;
    Information.Initialized = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkSmpSetProcessorResources(uint32_t LogicalId, uintptr_t KernelStackTop, uintptr_t PerCpuPointer)
{
    OesdkSmpProcessor *Processor;
    if (!Information.Initialized || LogicalId >= Information.DiscoveredProcessorCount || KernelStackTop == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    Processor = &Processors[LogicalId];
    if (!Processor->Enabled || Processor->BootstrapProcessor) return OESDK_STATUS_INVALID_ARGUMENT;
    if (__atomic_load_n(&Processor->State, __ATOMIC_ACQUIRE) >= OesdkCpuStartupStarting) return OESDK_STATUS_BUSY;
    Processor->KernelStackTop = KernelStackTop;
    Processor->PerCpuPointer = PerCpuPointer;
    __atomic_store_n(&Processor->State, OesdkCpuStartupPrepared, __ATOMIC_RELEASE);
    return OESDK_STATUS_SUCCESS;
}

static OesdkStatus StartOne(OesdkSmpProcessor *Processor)
{
    OesdkStatus Status;
    uint32_t Waited = 0U;
    uint8_t Vector = (uint8_t)(Configuration.TrampolineAddress >> 12U);
    if (__atomic_load_n(&Processor->State, __ATOMIC_ACQUIRE) != OesdkCpuStartupPrepared) return OESDK_STATUS_NOT_FOUND;
    Status = Configuration.PrepareTrampoline(Configuration.TrampolineAddress,
                                             Configuration.PageTableRoot,
                                             Configuration.ApplicationProcessorEntryPoint,
                                             Processor,
                                             Configuration.Context);
    if (OESDK_FAILED(Status)) goto Failed;
    __atomic_store_n(&Processor->State, OesdkCpuStartupStarting, __ATOMIC_RELEASE);
    Status = OesdkInterruptControllerSendInitIpi(Processor->ApicId);
    if (OESDK_FAILED(Status)) goto Failed;
    Delay(OESDK_SMP_INIT_DELAY_US);
    Status = OesdkInterruptControllerSendStartupIpi(Processor->ApicId, Vector);
    if (OESDK_FAILED(Status)) goto Failed;
    Delay(OESDK_SMP_SIPI_DELAY_US);
    if (__atomic_load_n(&Processor->State, __ATOMIC_ACQUIRE) != OesdkCpuStartupOnline) {
        Status = OesdkInterruptControllerSendStartupIpi(Processor->ApicId, Vector);
        if (OESDK_FAILED(Status)) goto Failed;
    }
    while (__atomic_load_n(&Processor->State, __ATOMIC_ACQUIRE) != OesdkCpuStartupOnline) {
        if (Waited >= Configuration.StartupTimeoutMicroseconds) { Status = OESDK_STATUS_TIMEOUT; goto Failed; }
        Delay(OESDK_SMP_POLL_DELAY_US);
        Waited += OESDK_SMP_POLL_DELAY_US;
    }
    return OESDK_STATUS_SUCCESS;
Failed:
    Processor->StartupStatus = Status;
    __atomic_store_n(&Processor->State, OesdkCpuStartupFailed, __ATOMIC_RELEASE);
    return Status;
}

OesdkStatus OesdkSmpStartApplicationProcessors(void)
{
    uint32_t Index;
    OesdkStatus Overall = OESDK_STATUS_SUCCESS;
    if (!Information.Initialized) return OESDK_STATUS_NOT_FOUND;
    for (Index = 0U; Index < Information.DiscoveredProcessorCount; ++Index) {
        OesdkSmpProcessor *Processor = &Processors[Index];
        OesdkStatus Status;
        if (!Processor->Enabled || Processor->BootstrapProcessor) continue;
        Status = StartOne(Processor);
        if (OESDK_FAILED(Status) && OESDK_SUCCEEDED(Overall)) Overall = Status;
    }
    Information.StartupComplete = true;
    return Overall;
}

void OesdkSmpApplicationProcessorOnline(uint32_t ApicId)
{
    OesdkSmpProcessor *Processor = FindByApicId(ApicId);
    uint32_t Expected = OesdkCpuStartupStarting;
    if (Processor == NULL || Processor->BootstrapProcessor) return;
    if (__atomic_compare_exchange_n(&Processor->State, &Expected, OesdkCpuStartupOnline, false,
                                    __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
        Processor->StartupStatus = OESDK_STATUS_SUCCESS;
        __atomic_add_fetch(&Information.OnlineProcessorCount, 1U, __ATOMIC_ACQ_REL);
    }
}

void OesdkSmpApplicationProcessorMain(uint32_t ApicId)
{
    OesdkSmpProcessor *Processor = FindByApicId(ApicId);
    if (Processor == NULL || Processor->KernelStackTop == 0U) {
        for (;;) OesdkCpuHalt();
    }
    OesdkSmpApplicationProcessorOnline(ApicId);
    if (Configuration.ApplicationProcessorEntry != NULL)
        Configuration.ApplicationProcessorEntry(Processor->LogicalId, Configuration.Context);
    for (;;) OesdkCpuHalt();
}

const OesdkSmpProcessor *OesdkSmpProcessorGet(uint32_t LogicalId)
{
    return Information.Initialized && LogicalId < Information.DiscoveredProcessorCount ? &Processors[LogicalId] : NULL;
}

uint32_t OesdkSmpCurrentApicId(void) { return OesdkInterruptControllerCurrentApicId(); }
const OesdkSmpProcessor *OesdkSmpCurrentProcessor(void) { return FindByApicId(OesdkSmpCurrentApicId()); }
bool OesdkSmpIsBootstrapProcessor(void) { const OesdkSmpProcessor *P = OesdkSmpCurrentProcessor(); return P != NULL && P->BootstrapProcessor; }
const OesdkSmpInformation *OesdkSmpInformationGet(void) { return Information.Initialized ? &Information : NULL; }
