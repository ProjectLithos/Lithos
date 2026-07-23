#!/usr/bin/env python3
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
files = [
    'VisualStudio/Templates/Kernel/UsageBoot.c',
    'Packages/VisualStudio/Templates/Kernel/UsageBoot.c',
    'InstallerSource/VisualStudio/Templates/Kernel/UsageBoot.c',
    'InstallerSource/Packages/VisualStudio/Templates/Kernel/UsageBoot.c',
]
for rel in files:
    text = (ROOT / rel).read_text(encoding='utf-8')
    assert '"ABI Version: " +' not in text, f'{rel}: unsafe string-plus-integer expression'
    assert 'Boot ABI=%u protocol=%u' in text, f'{rel}: safe numeric boot-context formatting example missing'
    assert '(unsigned int)BootContext->AbiVersion' in text, f'{rel}: ABI version is not passed as a formatted integer'
for rel in ['Tools/New-OESDKProject.ps1','Packages/x86_64/Tools/New-OESDKProject.ps1','InstallerSource/Tools/New-OESDKProject.ps1','InstallerSource/Packages/x86_64/Tools/New-OESDKProject.ps1']:
    text=(ROOT/rel).read_text(encoding='utf-8')
    assert 'VisualStudio\\Templates' in text and "Extension -in @('.c', '.h')" in text, f'{rel}: modular template copy logic missing'
print('[ OK ] Modular project templates format numeric boot-context fields safely')
