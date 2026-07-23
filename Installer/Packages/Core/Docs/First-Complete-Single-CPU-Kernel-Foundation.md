# First Complete Single-CPU Kernel Foundation

OESDK 0.18.0 is the first integrated single-CPU kernel foundation. It combines the previously delivered boot, diagnostics, processor, descriptor-table, interrupt, memory, heap, timer, scheduler, process, user-mode, system-call, IPC, and capability layers behind one public SDK surface.

## Foundation contract

The 0.18.0 foundation provides:

- x86-64 boot entry and linker layout.
- Boot-context validation and normalized firmware memory maps.
- Serial/console diagnostics, status values, assertions, panic handling, and page-fault decoding.
- CPU primitives, GDT, TSS, Ring 0 and Ring 3 descriptors, IDT, exceptions, interrupt dispatch, interrupt-controller abstraction, and timer abstraction.
- Bitmap physical-page allocation, page-table virtual memory, kernel heap allocation, and explicit user-page permissions.
- Single-CPU thread records, kernel stacks, saved contexts, priority, quantum accounting, round-robin scheduling, blocking, sleeping, waking, and termination.
- Processes, per-process address spaces, user stacks, ELF64 executable loading, controlled Ring 3 entry, user-pointer validation, and process termination.
- The generic Get, Set, and Event syscall ABI with validated user buffers and negative status returns.
- Channels, endpoints, events, capabilities, capability-rights reduction, and manifest-driven Kernel/User/Excluded/Automatic module placement.

## Single-CPU scope

This release intentionally uses one active processor and one scheduler run queue. It does not claim SMP startup, inter-processor interrupts, per-CPU run queues, cross-CPU TLB shootdown, or lock-free multi-CPU scheduling. Those are later architecture capabilities rather than hidden requirements of this foundation.

## Safety boundaries

- Userland never receives arbitrary kernel pointers.
- User memory is accepted only when the complete range is below the user/kernel boundary, does not overflow, and every covered page is present and user-accessible.
- Write operations require writable mappings.
- Capability derivation can only reduce rights: `ChildRights = ParentRights & RequestedRights`.
- ELF mappings preserve segment permissions and reject malformed or overflowing ranges.

## Version identity

- SDK identity: `0.18.0`
- Package identity: `0.18.0.0`
