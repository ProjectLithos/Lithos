# `OesdkVirtualUnmap`

```c
OesdkStatus OesdkVirtualUnmap(
    uintptr_t VirtualAddress,
    size_t PageCount);
```

Removes `PageCount` contiguous 4 KiB mappings. The start address must be aligned, the complete range must be canonical, and the page-count multiplication and end-address addition are checked for overflow before the page-table walk begins. Each removed leaf is invalidated with `invlpg`.

Declared in `<oesdk/virtual_memory.h>`.
