#!/usr/bin/env python3
from __future__ import annotations
import json, pathlib, re
ROOT = pathlib.Path(__file__).resolve().parents[1]
version = (ROOT / 'VERSION').read_text(encoding='utf-8').strip()
major, minor, patch = (int(part) for part in version.split('.'))
manifest = json.loads((ROOT / 'manifest.json').read_text(encoding='utf-8'))
assert manifest['sdkVersion'] == version
assert manifest['packageRevision'] == version
assert manifest['visualStudioSdkVersion'] == version
expected = {'MAJOR': major, 'MINOR': minor, 'PATCH': patch, 'REVISION': 0}
for rel in ('Include/oesdk/kernel.h','Packages/Core/Include/oesdk/kernel.h','InstallerSource/Packages/Core/Include/oesdk/kernel.h'):
    text=(ROOT/rel).read_text(encoding='utf-8')
    for name,value in expected.items():
        m=re.search(rf'#define\s+OESDK_VERSION_{name}\s+(\d+)', text)
        assert m and int(m.group(1)) == value, f'{rel}: stale {name}'
for rel in ('VisualStudio/VERSION','Packages/VisualStudio/VERSION','InstallerSource/Packages/VisualStudio/VERSION'):
    assert (ROOT/rel).read_text(encoding='utf-8').strip() == version
print(f'[ OK ] All version metadata consistently reports {version}')
