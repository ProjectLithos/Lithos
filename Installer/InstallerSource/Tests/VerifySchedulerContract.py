#!/usr/bin/env python3
from pathlib import Path
r=Path(__file__).resolve().parents[1]
h=(r/'Include/oesdk/scheduler.h').read_text(); c=(r/'Source/scheduler.c').read_text(); d=(r/'Docs/Threads-and-Scheduler.md').read_text()
for x in ['OesdkThreadStateCreated','OesdkThreadStateReady','OesdkThreadStateRunning','OesdkThreadStateBlocked','OesdkThreadStateSleeping','OesdkThreadStateTerminated','OesdkThreadContext','KernelStack','AddressSpaceOwner','WaitReason','OesdkSchedulerSelectNext','OesdkSchedulerOnTimerTick']:
 assert x in h or x in c
assert 'WholeSeconds = QuantumMilliseconds / 1000U' in c
assert 'RemainderMs = QuantumMilliseconds % 1000U' in c
assert 'RemainderProduct % 1000U' in c
assert '(Start + Offset) % OESDK_SCHEDULER_MAX_THREADS' in c
assert '15U' in h and 'first Ready thread after CurrentThread' in d
print('[ OK ] Threads and scheduler contract verified.')
