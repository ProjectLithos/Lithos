#!/usr/bin/env python3
from pathlib import Path
r=Path(__file__).resolve().parents[1]
h=(r/'Include/oesdk/heap.h').read_text(); c=(r/'Source/heap.c').read_text(); k=(r/'Include/oesdk/kernel.h').read_text(); d=(r/'Docs/Kernel-Heap.md').read_text()
for token in ['OesdkHeapBootstrapInitialize','OesdkHeapBootstrapAllocate','OesdkHeapBootstrapAllocateZeroed','OesdkHeapInformation']:
    assert token in h, token
assert 'Result = AlignUp(Current, Alignment)' in d
assert 'Next   = Result + RequestedSize' in d
assert 'if (Next < Result || Next > OesdkHeapState.End) return NULL;' in c
assert 'Count != 0U && Size > ((size_t)-1) / Count' in c
assert 'OesdkHeapState.Current = Next;' in c
assert 'Next >= Result' in d and 'Next <= HeapEnd' in d
assert 'cannot free' in d
assert '#include <oesdk/heap.h>' in k
for rel in ['Packages/Core/Source/heap.c','InstallerSource/Source/heap.c','InstallerSource/Packages/Core/Source/heap.c']:
    assert (r/rel).read_text() == c, rel
print('[ OK ] Bootstrap bump allocator contract verified.')
