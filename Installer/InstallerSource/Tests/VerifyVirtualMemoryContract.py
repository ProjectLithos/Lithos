#!/usr/bin/env python3
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
h=(ROOT/'Include/oesdk/virtual_memory.h').read_text()
c=(ROOT/'Source/virtual_memory.c').read_text()
d=(ROOT/'Docs/Virtual-Memory.md').read_text()
required=[
'OESDK_VIRTUAL_PAGE_SIZE 4096ULL','OesdkVirtualMap','OesdkVirtualUnmap','OesdkVirtualTranslate',
'OesdkVirtualAddressIsCanonical','OesdkVirtualAddressDecode','OesdkVirtualAddressParts',
'OESDK_X86_64_PML4_SHIFT 39U','OESDK_X86_64_PDPT_SHIFT 30U','OESDK_X86_64_PD_SHIFT 21U',
'OESDK_X86_64_PT_SHIFT 12U','OESDK_X86_64_INDEX_MASK 0x1FFULL','OESDK_X86_64_OFFSET_MASK 0xFFFULL',
'OESDK_X86_64_CANONICAL_BIT 47U','OESDK_KERNEL_HIGHER_HALF_BASE 0xFFFFFFFF80000000ULL',
'OESDK_VIRTUAL_FLAG_NO_EXECUTE','SplitTwoMibEntry','invlpg','mov %%cr3','EFER_NXE']
missing=[x for x in required if x not in h+c+d]
assert not missing, 'Missing virtual-memory contract: '+', '.join(missing)
assert '#include <oesdk/virtual_memory.h>' in (ROOT/'Include/oesdk/kernel.h').read_text()
assert 'retains the bootstrap identity map' in d
assert 'not relocated there yet' in d
print('[ OK ] x86-64 page-table manager contract verified.')
