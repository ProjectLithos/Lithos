# Virtual Memory

OESDK 0.17.18 adds an x86-64 page-table manager over the active CR3 address space. It retains the bootstrap identity map and permits reliable 4 KiB mappings before a later higher-half transition.

## Address layout

A 48-bit canonical address uses PML4, PDPT, PD, PT and 12-bit offset fields. `OesdkVirtualAddressIsCanonical` rejects addresses whose bits 63-48 do not sign-extend bit 47.

## API

- `OesdkVirtualMemoryInitialize` adopts the active CR3 root and enables EFER.NXE when supported.
- `OesdkVirtualMap` maps aligned 4 KiB pages with explicit writable, user, cache, global and no-execute flags.
- `OesdkVirtualUnmap` removes 4 KiB mappings and invalidates the TLB entry.
- `OesdkVirtualTranslate` resolves 4 KiB, 2 MiB and 1 GiB mappings.

The manager splits an existing 2 MiB bootstrap mapping into a 4 KiB page table when a page inside it must be remapped. Newly allocated page-table pages must currently reside below 4 GiB because that range is identity-mapped by the bootstrap.
