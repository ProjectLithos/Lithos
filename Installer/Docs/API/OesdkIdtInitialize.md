# `OesdkIdtInitialize`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Constructs and loads the 256-entry IDT and exception gates.

## Declaration

```c
bool OesdkIdtInitialize(void);
```

## Returns

`true` on success; otherwise `false`.

## Notes

- The runtime calls this after GDT/TSS initialization and before `kmain`.

## Example

```c
OESDK_ASSERT(OesdkIdtInitialize());
```

[Back to the API index](./README.md)
