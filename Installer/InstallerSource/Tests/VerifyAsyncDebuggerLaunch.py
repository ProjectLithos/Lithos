#!/usr/bin/env python3
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
text=(ROOT/'Tools/Debug-Kernel.ps1').read_text(encoding='utf-8')
assert '-Wait' not in text[text.index('$gdbConsole = Start-Process'):text.index('} catch {')]
assert 'Interactive GDB console started with process id' in text
assert 'taskkill /PID $($qemuProcess.Id)' in text
assert '-1073741510' in text
print('[ OK ] Interactive GDB sessions launch asynchronously and close QEMU cleanly.')
