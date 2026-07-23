#!/usr/bin/env python3
from pathlib import Path
r=Path(__file__).resolve().parents[1]
h=(r/'Include/oesdk/heap.h').read_text(); c=(r/'Source/heap.c').read_text(); k=(r/'Include/oesdk/kernel.h').read_text()
for token in ['OesdkHeapBootstrapInitialize','OesdkHeapBootstrapAllocate','OesdkHeapBootstrapAllocateZeroed','OesdkHeapInformation']:
    assert token in h, token
assert 'Count != 0U && Size > ((size_t)-1) / Count' in c
assert 'Next > OesdkHeapState.End' in c
assert '#include <oesdk/heap.h>' in k
print('[ OK ] Kernel heap contract verified.')
