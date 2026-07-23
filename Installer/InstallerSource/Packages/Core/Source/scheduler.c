#include <oesdk/scheduler.h>
#include <oesdk/heap.h>
#include <string.h>

static OesdkThread OesdkSchedulerThreads[OESDK_SCHEDULER_MAX_THREADS];
static OesdkSchedulerInformation OesdkSchedulerState;
static size_t OesdkSchedulerCurrentIndex = OESDK_SCHEDULER_MAX_THREADS;
static OesdkThreadId OesdkSchedulerNextId = UINT64_C(1);
static uint64_t OesdkSchedulerLastTick;

static OesdkThread *OesdkSchedulerFind(OesdkThreadId ThreadId, size_t *Index)
{
    size_t I;
    if (ThreadId == OESDK_THREAD_ID_INVALID) return NULL;
    for (I = 0U; I < OESDK_SCHEDULER_MAX_THREADS; ++I) {
        if (OesdkSchedulerThreads[I].Id == ThreadId) {
            if (Index != NULL) *Index = I;
            return &OesdkSchedulerThreads[I];
        }
    }
    return NULL;
}

static size_t OesdkSchedulerFirstUnused(void)
{
    size_t I;
    for (I = 0U; I < OESDK_SCHEDULER_MAX_THREADS; ++I) {
        if (OesdkSchedulerThreads[I].Id == OESDK_THREAD_ID_INVALID) return I;
    }
    return OESDK_SCHEDULER_MAX_THREADS;
}

uint32_t OesdkSchedulerQuantumTicks(uint32_t QuantumMilliseconds, uint32_t TimerHz)
{
    uint64_t WholeSeconds;
    uint64_t RemainderMs;
    uint64_t Ticks;
    uint64_t RemainderProduct;
    if (QuantumMilliseconds == 0U || TimerHz == 0U) return 0U;
    WholeSeconds = QuantumMilliseconds / 1000U;
    RemainderMs = QuantumMilliseconds % 1000U;
    if (WholeSeconds > UINT64_MAX / TimerHz) return UINT32_MAX;
    Ticks = WholeSeconds * TimerHz;
    RemainderProduct = RemainderMs * (uint64_t)TimerHz;
    Ticks += RemainderProduct / 1000U;
    if ((RemainderProduct % 1000U) != 0U) ++Ticks;
    if (Ticks == 0U) Ticks = 1U;
    return Ticks > UINT32_MAX ? UINT32_MAX : (uint32_t)Ticks;
}

OesdkStatus OesdkSchedulerInitialize(uint32_t TimerHz, uint32_t QuantumMilliseconds)
{
    uint32_t QuantumTicks;
    if (TimerHz == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    if (QuantumMilliseconds == 0U) QuantumMilliseconds = OESDK_SCHEDULER_DEFAULT_QUANTUM_MS;
    QuantumTicks = OesdkSchedulerQuantumTicks(QuantumMilliseconds, TimerHz);
    if (QuantumTicks == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    memset(OesdkSchedulerThreads, 0, sizeof(OesdkSchedulerThreads));
    memset(&OesdkSchedulerState, 0, sizeof(OesdkSchedulerState));
    OesdkSchedulerCurrentIndex = OESDK_SCHEDULER_MAX_THREADS;
    OesdkSchedulerNextId = UINT64_C(1);
    OesdkSchedulerLastTick = 0U;
    OesdkSchedulerState.TimerHz = TimerHz;
    OesdkSchedulerState.QuantumMilliseconds = QuantumMilliseconds;
    OesdkSchedulerState.QuantumTicks = QuantumTicks;
    OesdkSchedulerState.Initialized = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkThreadCreate(OesdkThreadEntry Entry, void *EntryContext, size_t KernelStackBytes,
                              uint8_t Priority, void *AddressSpaceOwner, OesdkThreadId *ThreadId)
{
    size_t Index;
    uintptr_t StackTop;
    OesdkThread *Thread;
    if (!OesdkSchedulerState.Initialized) return OESDK_STATUS_BUSY;
    if (Entry == NULL || ThreadId == NULL) return OESDK_STATUS_INVALID_ARGUMENT;
    if (KernelStackBytes == 0U) KernelStackBytes = OESDK_THREAD_DEFAULT_STACK_BYTES;
    if (KernelStackBytes < 1024U) return OESDK_STATUS_INVALID_ARGUMENT;
    Index = OesdkSchedulerFirstUnused();
    if (Index == OESDK_SCHEDULER_MAX_THREADS) return OESDK_STATUS_OUT_OF_MEMORY;
    Thread = &OesdkSchedulerThreads[Index];
    memset(Thread, 0, sizeof(*Thread));
    Thread->KernelStack = OesdkAllocateAligned(KernelStackBytes, 16U);
    if (Thread->KernelStack == NULL) return OESDK_STATUS_OUT_OF_MEMORY;
    StackTop = ((uintptr_t)Thread->KernelStack + KernelStackBytes) & ~(uintptr_t)0xFU;
    Thread->Id = OesdkSchedulerNextId++;
    if (OesdkSchedulerNextId == OESDK_THREAD_ID_INVALID) OesdkSchedulerNextId = UINT64_C(1);
    Thread->State = OesdkThreadStateCreated;
    Thread->Priority = Priority;
    Thread->QuantumTicks = OesdkSchedulerState.QuantumTicks;
    Thread->RemainingTicks = Thread->QuantumTicks;
    Thread->AddressSpaceOwner = AddressSpaceOwner;
    Thread->WaitReason = OesdkThreadWaitReasonNone;
    Thread->KernelStackBytes = KernelStackBytes;
    Thread->Entry = Entry;
    Thread->EntryContext = EntryContext;
    Thread->Context.Rip = (uintptr_t)Entry;
    Thread->Context.Rdi = (uintptr_t)EntryContext;
    Thread->Context.Rsp = StackTop;
    Thread->Context.Rbp = StackTop;
    Thread->Context.Cs = 0x08U;
    Thread->Context.Ss = 0x10U;
    Thread->Context.Rflags = 0x202U;
    ++OesdkSchedulerState.ThreadCount;
    *ThreadId = Thread->Id;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkThreadMakeReady(OesdkThreadId ThreadId)
{
    OesdkThread *Thread = OesdkSchedulerFind(ThreadId, NULL);
    if (Thread == NULL) return OESDK_STATUS_NOT_FOUND;
    if (Thread->State == OesdkThreadStateTerminated) return OESDK_STATUS_INVALID_ARGUMENT;
    Thread->State = OesdkThreadStateReady;
    Thread->WaitReason = OesdkThreadWaitReasonNone;
    Thread->WakeTick = 0U;
    Thread->RemainingTicks = Thread->QuantumTicks;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkThreadBlock(OesdkThreadId ThreadId, OesdkThreadWaitReason Reason)
{
    OesdkThread *Thread = OesdkSchedulerFind(ThreadId, NULL);
    if (Thread == NULL) return OESDK_STATUS_NOT_FOUND;
    if (Thread->State == OesdkThreadStateTerminated || Reason == OesdkThreadWaitReasonNone || Reason == OesdkThreadWaitReasonSleep)
        return OESDK_STATUS_INVALID_ARGUMENT;
    Thread->State = OesdkThreadStateBlocked;
    Thread->WaitReason = Reason;
    if (ThreadId == OesdkSchedulerState.CurrentThreadId) OesdkSchedulerState.ReschedulePending = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkThreadSleep(OesdkThreadId ThreadId, uint64_t SleepTicks)
{
    OesdkThread *Thread = OesdkSchedulerFind(ThreadId, NULL);
    uint64_t Now;
    if (Thread == NULL) return OESDK_STATUS_NOT_FOUND;
    if (Thread->State == OesdkThreadStateTerminated || SleepTicks == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    Now = OesdkSchedulerLastTick;
    Thread->WakeTick = UINT64_MAX - Now < SleepTicks ? UINT64_MAX : Now + SleepTicks;
    Thread->State = OesdkThreadStateSleeping;
    Thread->WaitReason = OesdkThreadWaitReasonSleep;
    if (ThreadId == OesdkSchedulerState.CurrentThreadId) OesdkSchedulerState.ReschedulePending = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkThreadTerminate(OesdkThreadId ThreadId)
{
    OesdkThread *Thread = OesdkSchedulerFind(ThreadId, NULL);
    if (Thread == NULL) return OESDK_STATUS_NOT_FOUND;
    Thread->State = OesdkThreadStateTerminated;
    Thread->WaitReason = OesdkThreadWaitReasonNone;
    if (ThreadId == OesdkSchedulerState.CurrentThreadId) OesdkSchedulerState.ReschedulePending = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkThreadInformationGet(OesdkThreadId ThreadId, const OesdkThread **Thread)
{
    OesdkThread *Found;
    if (Thread == NULL) return OESDK_STATUS_INVALID_ARGUMENT;
    Found = OesdkSchedulerFind(ThreadId, NULL);
    if (Found == NULL) return OESDK_STATUS_NOT_FOUND;
    *Thread = Found;
    return OESDK_STATUS_SUCCESS;
}

const OesdkThread *OesdkSchedulerCurrentThread(void)
{
    return OesdkSchedulerCurrentIndex < OESDK_SCHEDULER_MAX_THREADS ? &OesdkSchedulerThreads[OesdkSchedulerCurrentIndex] : NULL;
}

const OesdkThread *OesdkSchedulerSelectNext(void)
{
    size_t Offset;
    size_t Start = OesdkSchedulerCurrentIndex < OESDK_SCHEDULER_MAX_THREADS ? OesdkSchedulerCurrentIndex : OESDK_SCHEDULER_MAX_THREADS - 1U;
    for (Offset = 1U; Offset <= OESDK_SCHEDULER_MAX_THREADS; ++Offset) {
        size_t Index = (Start + Offset) % OESDK_SCHEDULER_MAX_THREADS;
        OesdkThread *Thread = &OesdkSchedulerThreads[Index];
        if (Thread->Id != OESDK_THREAD_ID_INVALID && Thread->State == OesdkThreadStateReady) {
            if (OesdkSchedulerCurrentIndex < OESDK_SCHEDULER_MAX_THREADS &&
                OesdkSchedulerThreads[OesdkSchedulerCurrentIndex].State == OesdkThreadStateRunning)
                OesdkSchedulerThreads[OesdkSchedulerCurrentIndex].State = OesdkThreadStateReady;
            OesdkSchedulerCurrentIndex = Index;
            Thread->State = OesdkThreadStateRunning;
            Thread->RemainingTicks = Thread->QuantumTicks;
            OesdkSchedulerState.CurrentThreadId = Thread->Id;
            OesdkSchedulerState.ReschedulePending = false;
            return Thread;
        }
    }
    return OesdkSchedulerCurrentThread();
}

void OesdkSchedulerOnTimerTick(uint64_t CurrentTick)
{
    size_t I;
    OesdkThread *Current;
    if (!OesdkSchedulerState.Initialized) return;
    OesdkSchedulerLastTick = CurrentTick;
    for (I = 0U; I < OESDK_SCHEDULER_MAX_THREADS; ++I) {
        OesdkThread *Thread = &OesdkSchedulerThreads[I];
        if (Thread->Id != OESDK_THREAD_ID_INVALID && Thread->State == OesdkThreadStateSleeping && CurrentTick >= Thread->WakeTick) {
            Thread->State = OesdkThreadStateReady;
            Thread->WaitReason = OesdkThreadWaitReasonNone;
            Thread->WakeTick = 0U;
            Thread->RemainingTicks = Thread->QuantumTicks;
        }
    }
    Current = OesdkSchedulerCurrentIndex < OESDK_SCHEDULER_MAX_THREADS ? &OesdkSchedulerThreads[OesdkSchedulerCurrentIndex] : NULL;
    if (Current == NULL || Current->State != OesdkThreadStateRunning) return;
    if (Current->RemainingTicks > 0U) --Current->RemainingTicks;
    if (Current->RemainingTicks == 0U) OesdkSchedulerState.ReschedulePending = true;
}

bool OesdkSchedulerReschedulePending(void) { return OesdkSchedulerState.ReschedulePending; }
void OesdkSchedulerRescheduleAcknowledged(void) { OesdkSchedulerState.ReschedulePending = false; }
const OesdkSchedulerInformation *OesdkSchedulerInformationGet(void) { return &OesdkSchedulerState; }
