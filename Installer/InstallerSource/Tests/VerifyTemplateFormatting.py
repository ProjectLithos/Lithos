#!/usr/bin/env python3
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
files = [
    'VisualStudio/Templates/Kernel/kmain.c',
    'Packages/VisualStudio/Templates/Kernel/kmain.c',
    'InstallerSource/Packages/VisualStudio/Templates/Kernel/kmain.c',
    'Tools/New-OESDKProject.ps1',
    'Packages/x86_64/Tools/New-OESDKProject.ps1',
    'InstallerSource/Packages/x86_64/Tools/New-OESDKProject.ps1',
]
for rel in files:
    text = (ROOT / rel).read_text(encoding='utf-8')
    assert '"ABI Version: " + ' not in text, f'{rel}: unsafe string-plus-integer expression'
    assert 'kprintf("ABI Version: %u\\n", (unsigned int)bootContext->AbiVersion);' in text, f'{rel}: safe ABI formatting example missing'
print('[ OK ] Project templates format numeric boot-context fields safely')
