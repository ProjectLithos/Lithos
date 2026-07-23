# SMP Startup

OESDK 0.18.2 adds reusable x86-64 symmetric multiprocessing startup support while retaining safe single-CPU fallback.

The boot processor supplies enabled APIC IDs discovered from ACPI MADT or equivalent firmware data. OESDK assigns stable logical IDs, distinguishes the bootstrap processor from application processors, and records each processor as Discovered, Prepared, Starting, Online, or Failed.

Each application processor requires a private kernel stack and may receive a per-CPU pointer. The platform trampoline callback patches a page-aligned startup page below 1 MiB with the active page-table root, AP stack, AP identity, and 64-bit entry point.

Startup follows the architectural INIT-SIPI-SIPI sequence. OESDK waits ten milliseconds after INIT, sends the first startup IPI, sends a second startup IPI when needed, and does not count a processor online until that processor executes the release-ordered online handshake. A bounded timeout marks non-responsive processors Failed without preventing the bootstrap processor from continuing.

The API supports xAPIC MMIO and x2APIC MSR IPI delivery. Duplicate APIC IDs, a missing bootstrap processor, unprepared AP stacks, invalid trampoline addresses, and unsupported interrupt-controller modes fail explicitly.

This release performs processor startup only. Fully parallel scheduling additionally requires per-CPU scheduler state, inter-processor reschedule interrupts, locking, cross-CPU TLB shootdown, and migration policy in later releases.
