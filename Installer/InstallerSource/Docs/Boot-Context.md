# OESDK Boot Context 0.0.16.1

`OesdkBootContextInitialize` validates the Multiboot 1 handoff before `kmain` runs. `OesdkBootContextGet` returns the immutable context.

The context contains the boot protocol and raw handoff address, linker-derived kernel image boundaries, bootstrap stack boundaries, and available memory when Multiboot supplies it. Memory-map entries of type 1 are summed with overflow and record-length checks.

```c
#include <oesdk/kernel.h>

void kmain(int argc, char *argv) {
    (void)argc; (void)argv;
    const OesdkBootContext *boot = OesdkBootContextGet();
    kprintf("ABI Version: %u\n", (unsigned int)boot->AbiVersion);
    kprintf("Kernel: %p-%p\n", (void *)boot->KernelPhysicalStart, (void *)boot->KernelPhysicalEnd);
}
```

## Printing numeric fields

`AbiVersion` is an integer. Pass it as a formatting argument; do not add it to a string literal.

```c
kprintf("ABI Version: %u\n", (unsigned int)bootContext->AbiVersion);
```

The expression `"ABI Version: " + bootContext->AbiVersion` performs pointer arithmetic and does not append the number.
