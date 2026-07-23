# Threads and Scheduler

OESDK 0.17.35 adds the first architecture-neutral thread table and round-robin scheduler policy. It supplies initial x86_64 thread context records and stack ownership while leaving the final interrupt-return context switch under kernel control.

## Initial thread state

Each `OesdkThread` contains a unique ID, kernel stack, saved CPU context, lifecycle state, priority, time slice, process/address-space owner, wait reason, wake tick, entry point, and entry context. New threads begin in `Created`; call `OesdkThreadMakeReady` to place them in the runnable set.

States are `Created`, `Ready`, `Running`, `Blocked`, `Sleeping`, and `Terminated`. Wait reasons distinguish sleeps, events, mutexes, semaphores, I/O, joins, and custom waits.

## Round robin

`OesdkSchedulerSelectNext` implements:

```text
NextThread = first Ready thread after CurrentThread
```

The scan wraps at the end of the fixed 64-slot thread table. Priority is stored now for later policy extensions; the initial selection rule is deliberately strict round robin.

## Quantum calculation

`OesdkSchedulerQuantumTicks` uses the overflow-safe split requested by the API:

```text
WholeSeconds = QuantumMilliseconds / 1000
RemainderMs  = QuantumMilliseconds % 1000
QuantumTicks = WholeSeconds * TimerHz
             + ceil(RemainderMs * TimerHz / 1000)
```

The default is 15 ms, within the practical first-quantum range of approximately 10–20 ms. A nonzero duration always produces at least one tick. Saturation produces `UINT32_MAX`.

## Timer integration

The OESDK timer interrupt calls `OesdkSchedulerOnTimerTick`. It wakes expired sleepers, decrements the running thread's remaining quantum, and sets `ReschedulePending` when the quantum expires. The kernel performs its architecture-specific context switch at a safe interrupt-return or cooperative yield point, then acknowledges the request.

## Initial context

Thread creation aligns the kernel stack to 16 bytes and prepares RIP, RDI, RSP, RBP, CS, SS, and RFLAGS for x86_64 kernel execution. The full public context record also reserves all general-purpose registers so interrupt/context-switch code can save and restore a complete state.
