# `OesdkVirtualMap`

```c
OesdkStatus OesdkVirtualMap(
    uintptr_t VirtualAddress,
    uintptr_t PhysicalAddress,
    size_t PageCount,
    uint64_t Flags);
```

Maps `PageCount` contiguous 4 KiB virtual pages to contiguous physical pages. Both addresses must be page-aligned and the virtual range must remain canonical.

`Flags` accepts only the explicit public flags: `Present`, `Writable`, `User`, `WriteThrough`, `CacheDisable`, `Global`, and `NoExecute`. A mapping created by this function is always present; `OESDK_VIRTUAL_FLAG_PRESENT` may be supplied explicitly for readability. Unknown flag bits are rejected.

Before any multiplication or address addition, the implementation verifies:

```text
MappedBytes = PageCount * 4096
PageCount <= UINTPTR_MAX / 4096
Start <= UINTPTR_MAX - (MappedBytes - 1)
```

The function returns `OESDK_STATUS_ALREADY_EXISTS` when a target 4 KiB leaf is already mapped and `OESDK_STATUS_NOT_SUPPORTED` when NX is requested but unavailable.

Declared in `<oesdk/virtual_memory.h>`.
