#!/usr/bin/env python3
from pathlib import Path
r=Path(__file__).resolve().parents[1]
h=(r/'Include/oesdk/interrupt_controller.h').read_text()
c=(r/'Source/interrupt_controller.c').read_text()
i=(r/'Source/interrupt.c').read_text()
t=(r/'Source/timer.c').read_text()
runtime=(r/'Source/runtime.c').read_text()
for token in ('OesdkInterruptControllerInitializePic','OesdkInterruptControllerInitializeLocalApic','OesdkInterruptControllerInitializeX2Apic','OesdkInterruptControllerAttachIoApic','OesdkInterruptControllerRouteIrq','OesdkInterruptControllerMaskIrq','OesdkInterruptControllerUnmaskIrq','OesdkInterruptControllerEndOfInterrupt'):
    assert token in h and token in c, token
assert 'PIC1_COMMAND' in c and 'PIC2_COMMAND' in c
assert 'PIC_EOI' in c
assert 'X2APIC_EOI_MSR' in c and 'APIC_EOI_OFFSET' in c
assert 'IOAPIC_REDTBL' in c
assert 'OesdkInterruptControllerEndOfInterrupt' in i
assert 'OesdkInterruptControllerUnmaskIrq(0U)' in t
assert 'OesdkInterruptControllerInitializePic' in runtime
assert 'OesdkTimerInitializePit(100U' in runtime
assert runtime.index('OesdkInterruptControllerInitializePic') < runtime.index('OesdkTimerInitializePit') < runtime.index('OesdkCpuEnableInterrupts')
print('[ OK ] Interrupt controller and timer integration contract verified.')
