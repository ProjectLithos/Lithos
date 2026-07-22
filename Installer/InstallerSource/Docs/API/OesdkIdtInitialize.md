# `OesdkIdtInitialize`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Constructs and loads the kernel-owned 256-entry x86-64 IDT.

## Declaration

```c
OesdkStatus OesdkIdtInitialize(void);
```

## Returns

- `OESDK_STATUS_SUCCESS` when the IDT is loaded.
- `OESDK_STATUS_NOT_FOUND` when the GDT/TSS foundation is unavailable.

## Notes

- Installs vectors 0-255.
- Uses dedicated IST stacks for double fault, NMI, and machine check.
- Preserves and restores the caller's interrupt-enable state.

## Example

```c
OesdkStatus status = OesdkIdtInitialize();
OESDK_ASSERT(OESDK_SUCCEEDED(status));
```

[Back to the API index](./README.md)
