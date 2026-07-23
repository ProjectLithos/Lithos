# OESDK Template Kernel

The template keeps `kmain.c` as a small startup coordinator and separates SDK integration into focused usage files:

- `UsageBoot.c` — boot context and memory-map normalisation
- `UsageCpu.c` — CPU, GDT, IDT and SMP status
- `UsageMemory.c` — physical memory, virtual memory and heaps
- `UsageInterrupts.c` — interrupt handlers, controller and timer APIs
- `UsageObjects.c` — scheduler status, capabilities, channels, endpoints and events
- `UsageGraphics.c` — optional framebuffer drawing

Subsystems that require project-specific policy or firmware data are detected and reported rather than started with invented configuration. This includes AP topology, process address-space roots, ELF images, syscall namespaces and user-mode entry points.
