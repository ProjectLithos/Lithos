# OESDK 0.18.0

OESDK is a freestanding x86-64 operating-system development kit. Version 0.18.0 is the first complete integrated single-CPU kernel foundation.

It includes boot context handling, diagnostics and panic support, CPU primitives, GDT/TSS, Ring 3 descriptors, IDT and exceptions, interrupt-controller and timer abstractions, physical and virtual memory, kernel heap, threads and round-robin scheduling, processes and address spaces, user stacks, ELF64 loading, controlled user-mode entry, validated user pointers, the generic Get/Set/Event syscall ABI, IPC, events, and capability rights.

## Public header

```c
#include <oesdk/kernel.h>
```

## Architecture scope

This release targets a single active x86-64 CPU. SMP startup, inter-processor interrupts, per-CPU scheduling, and cross-CPU TLB shootdown are intentionally reserved for later releases.

## Documentation

- [First complete single-CPU foundation](Docs/First-Complete-Single-CPU-Kernel-Foundation.md)
- [Complete API reference](Docs/API/README.md)
- [Threads and scheduler](Docs/Threads-and-Scheduler.md)
- [Processes, user mode, system calls, and IPC](Docs/Processes-User-Mode-System-Calls-IPC.md)

## Build and debugging

OESDK projects produce a symbol-rich `Build/Debug/kernel.elf`. Visual Studio can run QEMU normally or open the interactive GDB console through the supplied debugging tools.

## Version

- SDK identity: `0.18.0`
- Package identity: `0.18.0.0`
