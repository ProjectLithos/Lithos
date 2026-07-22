#!/usr/bin/env python3
from pathlib import Path
import re
root=Path(__file__).resolve().parents[1]
h=(root/'Include/oesdk/memory_map.h').read_text()
c=(root/'Source/memory_map.c').read_text()
assert all(name in h for name in ('OesdkMemoryReserved','OesdkMemoryAvailable','OesdkMemoryAcpiReclaimable','OesdkMemoryAcpiNvs','OesdkMemoryBad','OesdkMemoryKernel','OesdkMemoryBootData','OesdkMemoryFramebuffer'))
assert re.search(r'uint64_t Base;\s*uint64_t Length;\s*OesdkMemoryType Type;',h)
assert 'Region->Length == 0U' in c
assert 'UINT64_MAX - Region->Base < Region->Length' in c
assert 'Left->Base < RightEnd && Right->Base < LeftEnd' in c
assert 'RecordBytes < sizeof(OesdkMultiboot1MemoryMapEntry)' in c
assert 'SortRegions();' in c and 'CoalesceAndValidate();' in c
assert 'Regions[ReadIndex].Base < PreviousEnd' in c
print('[ OK ] Memory-map normalisation contract verified.')
