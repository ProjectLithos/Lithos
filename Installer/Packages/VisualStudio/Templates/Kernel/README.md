# OESDK Base Kernel Template

This native Clang C template demonstrates every core service currently linked by
OESDK 0.17.17. The OESDK runtime initializes the serial port, CPU feature layer,
kernel-owned GDT/TSS, IDT, boot context, console and graphics surface before
calling `kmain`.

The generated `Source/kmain.c` then demonstrates:

- package and boot-context version information;
- screen console and COM1 debug output;
- CPU feature discovery and CPUID queries;
- GDT/TSS and IDT information;
- status names, descriptions and success/failure checks;
- safe interrupt-handler registration and removal;
- Multiboot 1 memory-map normalisation and range validation;
- panic and assertion contracts;
- framebuffer detection, pixel and rectangle drawing;
- interrupt-state save/restore and the CPU pause primitive.

The template deliberately does not trigger a CPU exception or panic, enable an
interrupt controller, or initialize the optional separately built Newlib target.
Those operations require project-specific policy or additional subsystems.

Build Debug or Release in Visual Studio, then press F5 or use Start Without Debugging to
launch `kernel.elf` in QEMU. Debug builds send `kdebugf` output to COM1.
