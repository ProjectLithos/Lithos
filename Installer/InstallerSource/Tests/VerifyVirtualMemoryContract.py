#!/usr/bin/env python3
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
h=(ROOT/'Include/oesdk/virtual_memory.h').read_text()
c=(ROOT/'Source/virtual_memory.c').read_text()
required=['OESDK_VIRTUAL_PAGE_SIZE 4096ULL','OesdkVirtualMap','OesdkVirtualUnmap','OesdkVirtualTranslate','OesdkVirtualAddressIsCanonical','OESDK_VIRTUAL_FLAG_NO_EXECUTE','SplitTwoMibEntry','invlpg','mov %%cr3','EFER_NXE']
missing=[x for x in required if x not in h+c]
assert not missing, 'Missing virtual-memory contract: '+', '.join(missing)
assert '#include <oesdk/virtual_memory.h>' in (ROOT/'Include/oesdk/kernel.h').read_text()
print('[ OK ] Virtual-memory contract verified.')
