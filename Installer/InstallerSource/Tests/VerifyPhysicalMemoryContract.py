from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
h=(ROOT/'Include/oesdk/physical_memory.h').read_text()
c=(ROOT/'Source/physical_memory.c').read_text()
assert '#define OESDK_PAGE_SIZE 4096ULL' in h
assert '#define OESDK_PAGE_SHIFT 12U' in h
for token in ['OESDK_ALIGN_DOWN','OESDK_ALIGN_UP','OESDK_IS_POWER_OF_TWO','OesdkPhysicalMemoryInitialize','OesdkPhysicalMemoryAllocate','OesdkPhysicalMemoryFree']:
    assert token in h
for token in ['OesdkPhysicalMemoryReserve(0U, OESDK_PAGE_SIZE, true)','__image_start','__bss_end','__bootstrap_stack_bottom','__bootstrap_stack_top','__bootstrap_page_tables_start','__bootstrap_page_tables_end','AllocationBitmap','PermanentBitmap','MemoryMapAddress','FramebufferAddress']:
    assert token in c
print('[ OK ] Bitmap physical-memory contract verified.')
