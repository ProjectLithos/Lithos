# `OesdkGdtSetInterruptStack`

## Header

```c
#include <oesdk/gdt.h>
```

## Declaration

```c
bool OesdkGdtSetInterruptStack(uint8_t IstIndex, uintptr_t StackTop);
```

## Parameters

- `IstIndex`: one-based IST index in the range 1–7.
- `StackTop`: non-zero, 16-byte-aligned top of the replacement stack.

## Return value

Returns `true` when the initialized TSS was updated. Returns `false` for an uninitialized GDT, an invalid IST index, zero, or a misaligned stack top.

## Example

```c
OESDK_ASSERT(OesdkGdtSetInterruptStack(4, emergencyStackTop));
```
