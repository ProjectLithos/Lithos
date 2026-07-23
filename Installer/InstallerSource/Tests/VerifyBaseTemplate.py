from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CANONICAL = ROOT / 'VisualStudio/Templates/Kernel/kmain.c'
MIRRORS = [
    ROOT / 'Packages/VisualStudio/Templates/Kernel/kmain.c',
    ROOT / 'InstallerSource/Packages/VisualStudio/Templates/Kernel/kmain.c',
]

text = CANONICAL.read_text(encoding='utf-8')
for mirror in MIRRORS:
    assert mirror.read_text(encoding='utf-8') == text, f'{mirror}: base template differs from canonical template'

required = {
    'boot context': 'OesdkBootContextGet()',
    'status type': 'OesdkStatus Status',
    'status success': 'OESDK_SUCCEEDED(Status)',
    'status failure': 'OESDK_FAILED(Status)',
    'status name': 'OesdkStatusName(Status)',
    'status description': 'OesdkStatusDescription(Status)',
    'CPU features': 'OesdkCpuFeaturesGet()',
    'CPUID': 'OesdkCpuQuery(0U, 0U, &Registers)',
    'interrupt state': 'OesdkCpuInterruptStateSave()',
    'CPU pause': 'OesdkCpuPause()',
    'GDT': 'OesdkGdtInformationGet()',
    'IDT': 'OesdkIdtInformationGet()',
    'exception names': 'OesdkExceptionName(14U)',
    'interrupt registration': 'OesdkInterruptHandlerRegister(',
    'interrupt removal': 'OesdkInterruptHandlerUnregister(',
    'memory normalisation': 'OesdkMemoryMapNormalizeMultiboot1(',
    'memory region count': 'OesdkMemoryMapRegionCount()',
    'memory range validation': 'OesdkMemoryRangeEnd(Region, &End)',
    'physical allocator': 'OesdkPhysicalMemoryInitialize(',
    'physical allocation': 'OesdkPhysicalMemoryAllocate(',
    'physical free': 'OesdkPhysicalMemoryFree(',
    'virtual memory': 'OesdkVirtualMemoryInitialize()',
    'assertion': 'OESDK_ASSERT(',
    'assertion error code': 'OESDK_ASSERT_CODE(',
    'panic': 'OESDK_PANIC(',
    'console clear': 'kclear()',
    'console text': 'kputs(',
    'formatted console': 'kprintf(',
    'serial debug': 'kdebugf(',
    'graphics availability': 'kgfx_available()',
    'graphics surface': 'kgfx_screen()',
    'graphics rectangle': 'kgfx_rectangle(',
    'graphics pixel': 'kgfx_pixel(',
}
for name, token in required.items():
    assert token in text, f'base template does not demonstrate {name}: {token}'

assert 'OesdkLibcInitialize' not in text, 'optional separately linked Newlib must not be presented as core runtime initialization'
assert 'OesdkCpuEnableInterrupts()' not in text, 'template must not enable hardware interrupts before an interrupt controller exists'

for rel in [
    'Tools/New-OESDKProject.ps1',
    'Packages/x86_64/Tools/New-OESDKProject.ps1',
    'InstallerSource/Packages/x86_64/Tools/New-OESDKProject.ps1',
]:
    generated = (ROOT / rel).read_text(encoding='utf-8')
    for token in required.values():
        project_token = token.replace('$safeprojectname$', '__PROJECT_NAME__')
        assert project_token in generated, f'{rel}: generated base project omits {project_token}'

print('[ OK ] Base template covers every currently linked core OESDK subsystem.')
