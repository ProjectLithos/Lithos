#!/usr/bin/env python3
from pathlib import Path
import re
root=Path(__file__).resolve().parents[1]
h=(root/'Include/oesdk/interrupt.h').read_text()
c=(root/'Source/interrupt.c').read_text()
s=(root/'Architecture/x86_64/interrupt_stubs.S').read_text()
assert '#define OESDK_IDT_VECTOR_COUNT 256U' in h
assert 'OesdkPageFaultDecode' in h and 'OesdkPageFaultDecodeCurrent' in h and 'OesdkInterruptHandlerRegister' in h
assert all(name in h for name in ('OesdkDivideErrorHandler','OesdkInvalidOpcodeHandler','OesdkGeneralProtectionFaultHandler','OesdkPageFaultHandler','OesdkDoubleFaultHandler','OesdkMachineCheckHandler'))
assert re.search(r'uint64_t Rsp;\s*uint64_t Ss;', h)
assert 'OesdkCpuReadCr2()' in c
for name, shift in (('PRESENT',0),('WRITE',1),('USER',2),('RESERVED',3),('INSTRUCTION',4),('PROTECTION_KEY',5),('SHADOW_STACK',6),('SGX',15)):
    assert f'#define OESDK_PAGE_FAULT_{name}_BIT' in h
    assert f'<< {shift}U' in h
for field in ('PresentViolation','WriteAccess','UserAccess','ReservedBitViolation','InstructionFetch','ProtectionKeyViolation','ShadowStackAccess','SgxViolation'):
    assert field in h and field in c
assert 'OesdkPageFaultDecodeCurrent(Frame->ErrorCode)' in c
assert all(f'case {v}U:' in c for v in (0,6,8,13,14,18))
assert all(f'OesdkInterruptStub{i}:' in s for i in range(256))
assert len(re.findall(r'^    \.quad OesdkInterruptStub\d+$',s,re.M)) == 256
for v in (8,10,11,12,13,14,17,21,29,30):
    block=s.split(f'OesdkInterruptStub{v}:',1)[1].split('OesdkInterruptStub',1)[0]
    assert 'pushq $0' not in block
print('[ OK ] Exceptions and interrupts contract verified.')
