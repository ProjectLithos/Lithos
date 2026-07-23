#!/usr/bin/env python3
from pathlib import Path
r=Path(__file__).resolve().parents[1]
h=(r/'Include/oesdk/timer.h').read_text(); c=(r/'Source/timer.c').read_text(); d=(r/'Docs/Timer.md').read_text()
for x in ['1193182U','OesdkTimerSourcePit','OesdkTimerSourceLocalApic','OesdkTimerSourceX2Apic','OesdkTimerSourceHpet','OesdkTimerInitializePit','OesdkTimerInitializeLocalApic','OesdkTimerInitializeX2Apic','OesdkTimerInitializeHpet','OesdkTimerTicksToNanoseconds']:
 assert x in h or x in c
assert '(OESDK_PIT_BASE_FREQUENCY + DesiredHz / 2U) / DesiredHz' in c
assert 'Seconds = Ticks / Hz' in c and 'Remainder = Ticks % Hz' in c
assert '1,193,182 Hz' in d
print('[ OK ] Timer contract verified.')
