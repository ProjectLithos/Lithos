# OESDK 0.18.0

First complete integrated single-CPU kernel foundation.

- Consolidated boot, diagnostics, CPU, GDT/TSS, IDT, exceptions, interrupts, memory management, heap, timer, scheduler, processes, Ring 3, ELF loading, system calls, IPC, and capabilities into one supported foundation.
- Expanded the public umbrella header to expose all completed foundation APIs.
- Declared the deliberate single-CPU boundary and the later SMP work that is not falsely claimed by this release.
- Standardized SDK identity as `0.18.0` and package identity as `0.18.0.0`.
