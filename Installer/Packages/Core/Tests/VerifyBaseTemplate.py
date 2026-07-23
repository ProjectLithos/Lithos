from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CANONICAL = ROOT / 'VisualStudio/Templates/Kernel'
MIRRORS = [
    ROOT / 'Packages/VisualStudio/Templates/Kernel',
    ROOT / 'InstallerSource/VisualStudio/Templates/Kernel',
    ROOT / 'InstallerSource/Packages/VisualStudio/Templates/Kernel',
]
FILES = ['kmain.c', 'Usage.h', 'UsageBoot.c', 'UsageCpu.c', 'UsageMemory.c',
         'UsageInterrupts.c', 'UsageObjects.c', 'UsageGraphics.c',
         'OESDKKernel.vcxproj', 'OESDKKernel.vstemplate', 'README.md']
for mirror in MIRRORS:
    for name in FILES:
        assert (mirror / name).read_text(encoding='utf-8') == (CANONICAL / name).read_text(encoding='utf-8'), \
            f'{mirror / name}: template differs from canonical template'

sources = '\n'.join((CANONICAL / name).read_text(encoding='utf-8') for name in FILES if name.endswith(('.c','.h')))
main = (CANONICAL / 'kmain.c').read_text(encoding='utf-8')
required = {
    'boot context': 'OesdkBootContextGet()',
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
    'SMP status': 'OesdkSmpInformationGet()',
    'interrupt registration': 'OesdkInterruptHandlerRegister(',
    'interrupt removal': 'OesdkInterruptHandlerUnregister(',
    'memory normalisation': 'OesdkMemoryMapNormalizeMultiboot1(',
    'physical allocator': 'OesdkPhysicalMemoryInitialize(',
    'physical allocation': 'OesdkPhysicalMemoryAllocate(',
    'physical free': 'OesdkPhysicalMemoryFree(',
    'virtual memory': 'OesdkVirtualMemoryInitialize()',
    'bootstrap heap': 'OesdkHeapBootstrapInitialize(',
    'permanent heap': 'OesdkHeapInitialize(',
    'heap allocation': 'OesdkAllocate(',
    'zeroed heap allocation': 'OesdkAllocateZeroed(',
    'heap free': 'OesdkFree(',
    'scheduler status': 'OesdkSchedulerInformationGet()',
    'capability creation': 'OesdkCapabilityCreate(',
    'capability reduction': 'OesdkCapabilityDerive(',
    'channel': 'OesdkChannelInitialize(',
    'endpoint send': 'OesdkEndpointSend(',
    'endpoint receive': 'OesdkEndpointReceive(',
    'event': 'OesdkEventInitialize(',
    'assertion': 'OESDK_ASSERT(',
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
    assert token in sources, f'base template does not demonstrate {name}: {token}'

for module in ['TemplateUsageBoot', 'TemplateUsageCpu', 'TemplateUsageMemory',
               'TemplateUsageInterruptsAndTimer', 'TemplateUsageObjects', 'TemplateUsageGraphics']:
    assert f'{module}(' in main, f'kmain does not coordinate {module}'
assert len(main.splitlines()) < 45, 'kmain.c should remain a small coordinator'
assert 'OesdkLibcInitialize' not in sources
assert 'OesdkCpuEnableInterrupts()' not in sources
print('[ OK ] Modular base template covers all safely usable linked OESDK subsystems.')
