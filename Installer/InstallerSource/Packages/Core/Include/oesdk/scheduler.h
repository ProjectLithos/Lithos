#ifndef OESDK_SCHEDULER_H
#define OESDK_SCHEDULER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_SCHEDULER_MAX_THREADS 64U
#define OESDK_THREAD_DEFAULT_STACK_BYTES (16U * 1024U)
#define OESDK_SCHEDULER_DEFAULT_QUANTUM_MS 15U
#define OESDK_THREAD_ID_INVALID UINT64_C(0)

typedef uint64_t OesdkThreadId;
typedef void (*OesdkThreadEntry)(void *Context);

typedef enum OesdkThreadState
{
    OesdkThreadStateCreated = 0,
    OesdkThreadStateReady,
    OesdkThreadStateRunning,
    OesdkThreadStateBlocked,
    OesdkThreadStateSleeping,
    OesdkThreadStateTerminated
} OesdkThreadState;

typedef enum OesdkThreadWaitReason
{
    OesdkThreadWaitReasonNone = 0,
    OesdkThreadWaitReasonSleep,
    OesdkThreadWaitReasonEvent,
    OesdkThreadWaitReasonMutex,
    OesdkThreadWaitReasonSemaphore,
    OesdkThreadWaitReasonIo,
    OesdkThreadWaitReasonJoin,
    OesdkThreadWaitReasonCustom
} OesdkThreadWaitReason;

typedef struct OesdkThreadContext
{
    uint64_t R15, R14, R13, R12, R11, R10, R9, R8;
    uint64_t Rdi, Rsi, Rbp, Rbx, Rdx, Rcx, Rax;
    uint64_t Rip, Cs, Rflags, Rsp, Ss;
} OesdkThreadContext;

typedef struct OesdkThread
{
    OesdkThreadId Id;
    OesdkThreadState State;
    uint8_t Priority;
    uint32_t QuantumTicks;
    uint32_t RemainingTicks;
    void *AddressSpaceOwner;
    OesdkThreadWaitReason WaitReason;
    uint64_t WakeTick;
    void *KernelStack;
    size_t KernelStackBytes;
    OesdkThreadContext Context;
    OesdkThreadEntry Entry;
    void *EntryContext;
} OesdkThread;

typedef struct OesdkSchedulerInformation
{
    uint32_t TimerHz;
    uint32_t QuantumMilliseconds;
    uint32_t QuantumTicks;
    size_t ThreadCount;
    OesdkThreadId CurrentThreadId;
    bool Initialized;
    bool ReschedulePending;
} OesdkSchedulerInformation;

uint32_t OesdkSchedulerQuantumTicks(uint32_t QuantumMilliseconds, uint32_t TimerHz);
OesdkStatus OesdkSchedulerInitialize(uint32_t TimerHz, uint32_t QuantumMilliseconds);
OesdkStatus OesdkThreadCreate(OesdkThreadEntry Entry, void *EntryContext, size_t KernelStackBytes,
                              uint8_t Priority, void *AddressSpaceOwner, OesdkThreadId *ThreadId);
OesdkStatus OesdkThreadMakeReady(OesdkThreadId ThreadId);
OesdkStatus OesdkThreadBlock(OesdkThreadId ThreadId, OesdkThreadWaitReason Reason);
OesdkStatus OesdkThreadSleep(OesdkThreadId ThreadId, uint64_t SleepTicks);
OesdkStatus OesdkThreadTerminate(OesdkThreadId ThreadId);
OesdkStatus OesdkThreadInformationGet(OesdkThreadId ThreadId, const OesdkThread **Thread);
const OesdkThread *OesdkSchedulerCurrentThread(void);
const OesdkThread *OesdkSchedulerSelectNext(void);
void OesdkSchedulerOnTimerTick(uint64_t CurrentTick);
bool OesdkSchedulerReschedulePending(void);
void OesdkSchedulerRescheduleAcknowledged(void);
const OesdkSchedulerInformation *OesdkSchedulerInformationGet(void);

#ifdef __cplusplus
}
#endif

#endif
