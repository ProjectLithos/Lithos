#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
launcher = (ROOT / 'Tools' / 'VisualStudio-Launch.ps1').read_text(encoding='utf-8')
assert 'IsDebuggerPresent' in launcher
assert 'Debug-Kernel.ps1' in launcher
assert 'Run-Kernel.ps1' in launcher

for relative in (
    'VisualStudio/Templates/Kernel/OESDKKernel.vcxproj',
    'VisualStudio/Templates/Desktop/OESDKDesktop.vcxproj',
    'Packages/VisualStudio/Templates/Kernel/OESDKKernel.vcxproj',
    'Packages/VisualStudio/Templates/Desktop/OESDKDesktop.vcxproj',
):
    text = (ROOT / relative).read_text(encoding='utf-8')
    assert 'VisualStudio-Launch.ps1' in text, relative
    assert '<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>' in text, relative

for relative in (
    'Tools/New-OESDKProject.ps1',
    'Packages/x86_64/Tools/New-OESDKProject.ps1',
):
    assert 'VisualStudio-Launch.ps1' in (ROOT / relative).read_text(encoding='utf-8'), relative

print('[ OK ] Visual Studio toolbar launch contract.')
