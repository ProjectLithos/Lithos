# `OesdkIdtInformationGet`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Returns information about the active IDT.

## Declaration

```c
const OesdkIdtInformation *OesdkIdtInformationGet(void);
```

## Returns

Pointer to the read-only `OesdkIdtInformation` record.

## Example

```c
kprintf("IDT base: 0x%llx\n", OesdkIdtInformationGet()->Base);
```

[Back to the API index](./README.md)
