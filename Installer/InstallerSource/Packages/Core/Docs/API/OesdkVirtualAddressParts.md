# `OesdkVirtualAddressParts`

Decoded fields of a 48-bit x86-64 virtual address.

- `Pml4Index`: bits 47-39.
- `PdptIndex`: bits 38-30.
- `PdIndex`: bits 29-21.
- `PtIndex`: bits 20-12.
- `Offset`: bits 11-0.
- `UpperHalf`: bit 47 is set.
- `Canonical`: bits 63-48 correctly sign-extend bit 47.
