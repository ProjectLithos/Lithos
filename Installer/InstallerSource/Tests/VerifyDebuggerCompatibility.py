#!/usr/bin/env python3
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
for rel in ("VisualStudio/Templates/Kernel/kmain.c","Packages/VisualStudio/Templates/Kernel/kmain.c"):
 t=(ROOT/rel).read_text(); assert "__attribute__((aligned" not in t; assert "_Alignas(4096)" not in t; assert "static unsigned char TemplateBootstrapHeap[256U * 1024U];" in t; assert "static unsigned char TemplatePermanentHeap[1024U * 1024U];" in t
print("[ OK ] Visual Studio template alignment is portable")
