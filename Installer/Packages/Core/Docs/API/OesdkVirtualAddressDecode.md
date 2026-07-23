# `OesdkVirtualAddressDecode`

```c
void OesdkVirtualAddressDecode(
    uintptr_t VirtualAddress,
    OesdkVirtualAddressParts *Parts);
```

Decodes an x86-64 virtual address into PML4, PDPT, PD, PT and page-offset fields. The output also reports whether the address is in the upper half and whether it satisfies the 48-bit canonical-address rule. A null `Parts` pointer is ignored.
