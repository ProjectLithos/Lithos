#!/usr/bin/env python3
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
for rel in ('VisualStudio/Templates/Kernel/UsageMemory.c','Packages/VisualStudio/Templates/Kernel/UsageMemory.c','InstallerSource/VisualStudio/Templates/Kernel/UsageMemory.c','InstallerSource/Packages/VisualStudio/Templates/Kernel/UsageMemory.c'):
 t=(ROOT/rel).read_text(); assert '__attribute__((aligned' not in t; assert '_Alignas(4096)' not in t; assert 'static unsigned char TemplateBootstrapHeap[256U * 1024U];' in t; assert 'static unsigned char TemplatePermanentHeap[1024U * 1024U];' in t
text=(ROOT/'Tools/Debug-Kernel.ps1').read_text(); target=text.index("'target remote 127.0.0.1:")
for c in ('set remote hostio-setfs-packet off','set remote verbose-resume-packet off','set remote noack-packet off'): assert c in text and text.index(c)<target
text=(ROOT/'Tools/Oesdk-DebugTools.ps1').read_text(); assert text.index("C:\\msys64\\ucrt64\\bin\\gdb.exe") < text.index("C:\\msys64\\ucrt64\\bin\\gdb-multiarch.exe")
print('[ OK ] Debugger and modular template compatibility verified')
