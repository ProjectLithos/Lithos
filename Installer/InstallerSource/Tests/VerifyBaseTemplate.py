from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
text = (ROOT / 'Packages/VisualStudio/Templates/Kernel/kmain.c').read_text(encoding='utf-8')
generator = (ROOT / 'Packages/x86_64/Tools/New-OESDKProject.ps1').read_text(encoding='utf-8')
required = [
    'OesdkBootContextGet()', 'OesdkStatusName(Status)', 'OesdkCpuFeaturesGet()',
    'OesdkCpuQuery(0U, 0U, &Registers)', 'OesdkGdtInformationGet()',
    'OesdkIdtInformationGet()', 'OesdkInterruptHandlerRegister(',
    'OesdkMemoryMapNormalizeMultiboot1(', 'OESDK_ASSERT(', 'OESDK_PANIC(',
    'kclear()', 'kputs(', 'kprintf(', 'kdebugf(', 'kgfx_available()',
    'kgfx_screen()', 'kgfx_rectangle(', 'kgfx_pixel('
]
for token in required:
    assert token in text, f'base template omits {token}'
    assert token in generator, f'project generator omits {token}'
assert 'OesdkLibcInitialize' not in text
assert 'OesdkCpuEnableInterrupts()' not in text
print('[ OK ] Base template covers every currently linked core OESDK subsystem.')
