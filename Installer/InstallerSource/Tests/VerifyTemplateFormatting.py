#!/usr/bin/env python3
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
files = [
    'Packages/VisualStudio/Templates/Kernel/kmain.c',
    'Packages/x86_64/Tools/New-OESDKProject.ps1',
]
for rel in files:
    text = (ROOT / rel).read_text(encoding='utf-8')
    assert '"ABI Version: " +' not in text, f'{rel}: unsafe string-plus-integer expression'
    assert '"Boot ABI: %u, protocol: %u\\n"' in text, f'{rel}: safe numeric boot-context formatting example missing'
    assert '(unsigned int)BootContext->AbiVersion' in text, f'{rel}: ABI version is not passed as a formatted integer'
print('[ OK ] Project templates format numeric boot-context fields safely')
