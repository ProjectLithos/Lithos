#!/usr/bin/env python3
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
header = (ROOT / 'Include/oesdk/gdt.h').read_text(encoding='utf-8')
source = (ROOT / 'Source/gdt.c').read_text(encoding='utf-8')
doc = (ROOT / 'Docs/GDT-and-TSS.md').read_text(encoding='utf-8')

selectors = {
    'OESDK_GDT_NULL_SELECTOR': '0x00U',
    'OESDK_GDT_KERNEL_CODE_SELECTOR': '0x08U',
    'OESDK_GDT_KERNEL_DATA_SELECTOR': '0x10U',
    'OESDK_GDT_USER_DATA_SELECTOR': '0x18U',
    'OESDK_GDT_USER_CODE_SELECTOR': '0x20U',
    'OESDK_GDT_TSS_SELECTOR': '0x28U',
}
for name, value in selectors.items():
    assert re.search(rf'#define\s+{name}\s+{re.escape(value)}\b', header), f'missing selector {name}={value}'

assert 'OESDK_GDT_IST_ENTRY_COUNT    7U' in header
assert 'OesdkIstStacks[OESDK_GDT_IST_ENTRY_COUNT][OESDK_GDT_IST_STACK_SIZE]' in source
assert 'for (size_t Index = 0U; Index < OESDK_GDT_IST_ENTRY_COUNT; ++Index)' in source
assert 'OesdkTss.IoMapBase = (uint16_t)sizeof(OesdkTaskStateSegment);' in source
assert '__asm__ volatile("ltr %0"' in source
assert '"lgdt (%0)\\n"' in source
assert 'OesdkGdtSetInterruptStack' in header and 'OesdkGdtSetInterruptStack' in source
assert all(token in doc for token in ('0x00', '0x08', '0x10', '0x18', '0x20', '0x28'))
print('[ OK ] Complete GDT/TSS contract verified.')
