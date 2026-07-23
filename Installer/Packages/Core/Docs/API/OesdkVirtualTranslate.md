# `OesdkVirtualTranslate`

```c
bool OesdkVirtualTranslate(
    uintptr_t VirtualAddress,
    uintptr_t *PhysicalAddress);
```

Translates a canonical virtual address through the active x86-64 page tables. The returned physical address includes the original offset and supports existing 4 KiB, 2 MiB, and 1 GiB mappings. Returns `false` when the address is invalid or unmapped.

Declared in `<oesdk/virtual_memory.h>`.
