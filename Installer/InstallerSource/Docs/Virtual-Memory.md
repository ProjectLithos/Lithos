# x86-64 Page-Table Manager

OESDK 0.17.21 provides an explicit x86-64 page-table manager over the active CR3 address space. It deliberately retains the bootstrap identity map. A higher-half kernel relocation is deferred until mapping, translation, invalidation and allocation are dependable.

## 48-bit virtual-address layout

```
63                   48 47       39 38       30 29       21 20       12 11      0
+----------------------+-----------+-----------+-----------+-----------+----------+
| sign extension       | PML4      | PDPT      | PD        | PT        | offset   |
+----------------------+-----------+-----------+-----------+-----------+----------+
                         9 bits      9 bits      9 bits      9 bits      12 bits
```

The public constants define the shifts and masks. `OesdkVirtualAddressDecode` applies these formulas:

```
Pml4Index = (VirtualAddress >> 39) & 0x1FF
PdptIndex = (VirtualAddress >> 30) & 0x1FF
PdIndex   = (VirtualAddress >> 21) & 0x1FF
PtIndex   = (VirtualAddress >> 12) & 0x1FF
Offset    =  VirtualAddress        & 0xFFF
```

`OesdkVirtualAddressParts` returns all five fields, whether the address selects the upper half, and whether it is canonical.

## Canonical addresses

For 48-bit addressing, bits 63-48 must equal bit 47. Therefore the valid ranges are:

- `0x0000000000000000` through `0x00007FFFFFFFFFFF`
- `0xFFFF800000000000` through `0xFFFFFFFFFFFFFFFF`

`OesdkVirtualAddressIsCanonical` enforces this rule. `OESDK_KERNEL_HIGHER_HALF_BASE` is defined as `0xFFFFFFFF80000000`, but the current kernel is not relocated there yet.

## Mapper API

- `OesdkVirtualMemoryInitialize` adopts the active CR3 root and enables EFER.NXE when supported.
- `OesdkVirtualMap` uses the exact public mapping signature and accepts explicit Present, Writable, User, WriteThrough, CacheDisable, Global and NoExecute flags.
- `OesdkVirtualUnmap` removes 4 KiB mappings and invalidates each TLB entry.
- `OesdkVirtualTranslate` resolves existing 4 KiB, 2 MiB and 1 GiB mappings.
- `OesdkVirtualAddressDecode` exposes the PML4, PDPT, PD, PT and offset split without walking tables.

The manager splits an existing 2 MiB bootstrap mapping into a 4 KiB page table when a page inside it must be remapped. Newly allocated page-table pages must currently reside below 4 GiB because that range remains identity-mapped by the bootstrap.

## Current layout policy

The active bootstrap identity map is preserved. `OESDK_KERNEL_HIGHER_HALF_BASE` is a layout constant for a later revision, not evidence that the current kernel has already moved. This avoids combining a relocation change with the initial mapper implementation.

## Overflow-safe page ranges

Both mapping and unmapping validate the complete byte range before changing a page table. The implementation checks `PageCount <= UINTPTR_MAX / 4096` before calculating `MappedBytes = PageCount * 4096`, then verifies that adding `MappedBytes - 1` to the start address cannot wrap. Unknown mapping flag bits are rejected.
