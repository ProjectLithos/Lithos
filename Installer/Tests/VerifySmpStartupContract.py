#!/usr/bin/env python3
from pathlib import Path
R=Path(__file__).resolve().parents[1]
h=(R/'Include/oesdk/smp.h').read_text(); s=(R/'Source/smp.c').read_text(); ic=(R/'Source/interrupt_controller.c').read_text(); k=(R/'Include/oesdk/kernel.h').read_text()
for token in ('OesdkCpuStartupPrepared','OesdkCpuStartupStarting','OesdkCpuStartupOnline','OesdkCpuStartupFailed','OesdkSmpStartApplicationProcessors','OesdkSmpApplicationProcessorOnline'):
    assert token in h or token in s, token
for token in ('OesdkInterruptControllerSendInitIpi','OesdkInterruptControllerSendStartupIpi','APIC_ICR_DELIVERY_INIT','APIC_ICR_DELIVERY_STARTUP'):
    assert token in ic, token
assert '__atomic_compare_exchange_n' in s
assert 'StartupTimeoutMicroseconds' in s
assert '#include <oesdk/smp.h>' in k
assert (R/'Packages/x86_64/Architecture/x86_64/ap_trampoline.S').is_file()
print('[ OK ] SMP startup contract covers INIT-SIPI-SIPI, AP stacks, handshake, timeout, and fallback')
