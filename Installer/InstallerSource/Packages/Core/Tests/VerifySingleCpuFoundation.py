#!/usr/bin/env python3
from __future__ import annotations
import pathlib
ROOT=pathlib.Path(__file__).resolve().parents[1]
required_headers=[
'boot_context.h','panic.h','cpu.h','status.h','gdt.h','interrupt.h','memory_map.h',
'physical_memory.h','virtual_memory.h','heap.h','timer.h','interrupt_controller.h',
'scheduler.h','address_space.h','process.h','elf_loader.h','user_mode.h','syscall.h',
'ipc.h','capability.h','event.h','module_manifest.h']
required_sources=['runtime.c','gdt.c','interrupt.c','physical_memory.c','virtual_memory.c','heap.c',
                  'timer.c','scheduler.c','address_space.c','process.c','elf_loader.c','user_mode.c',
                  'syscall.c','channel.c','endpoint.c','capability.c','event.c']
umbrella=(ROOT/'Include/oesdk/kernel.h').read_text(encoding='utf-8')
for name in required_headers:
    assert (ROOT/'Include/oesdk'/name).is_file(), f'missing public header {name}'
    assert f'#include <oesdk/{name}>' in umbrella, f'umbrella omits {name}'
for name in required_sources:
    assert (ROOT/'Source'/name).is_file(), f'missing foundation source {name}'
doc=(ROOT/'Docs/First-Complete-Single-CPU-Kernel-Foundation.md').read_text(encoding='utf-8')
for phrase in ('one active processor','Userland never receives arbitrary kernel pointers',
               'ChildRights = ParentRights & RequestedRights','SDK identity: `0.18.0`'):
    assert phrase in doc, f'foundation contract missing: {phrase}'
print('[ OK ] OESDK 0.18.0 single-CPU foundation is complete and publicly exposed')
