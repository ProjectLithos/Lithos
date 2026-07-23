#!/usr/bin/env python3
from pathlib import Path
r=Path(__file__).resolve().parents[1]
h=(r/'Include/oesdk/heap.h').read_text(); c=(r/'Source/heap.c').read_text(); d=(r/'Docs/Kernel-Heap.md').read_text()
for token in ['typedef struct OesdkHeapBlock','uint64_t Magic','size_t Size','bool Free','OesdkHeapBlock *Previous','OesdkHeapBlock *Next','OesdkHeapInitialize','OesdkAllocate(size_t Size)','OesdkAllocateAligned','OesdkAllocateZeroed','OesdkFree']:
    assert token in h, token
assert 'OESDK_HEAP_BLOCK_MAGIC' in h
assert 'OESDK_HEAP_MINIMUM_BLOCK_SIZE' in h
assert 'Count != 0U && Size > ((size_t)-1) / Count' in c
assert 'BlockEndAddress(Block) == BlockAddress(Next)' in c
assert 'SuffixTotal >= sizeof(OesdkHeapBlock) + OESDK_HEAP_MINIMUM_BLOCK_SIZE' in c
assert 'MergeWithNext(Block)' in c
assert 'OriginalSize >=' in d and 'MinimumBlockSize' in d
for rel in ['Packages/Core/Source/heap.c','InstallerSource/Source/heap.c','InstallerSource/Packages/Core/Source/heap.c']:
    assert (r/rel).read_text() == c, rel
print('[ OK ] Permanent free-list heap contract verified.')
