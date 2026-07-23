# `OesdkGdtInformationGet`

**Category:** GDT and TSS  
**Kind:** function  
**Header:** `<oesdk/gdt.h>`

Returns information about the active GDT and TSS stacks.

## Declaration

```c
const OesdkGdtInformation *OesdkGdtInformationGet(void);
```

## Returns

Pointer to the read-only `OesdkGdtInformation` record.

## Example

```c
kprintf("TSS selector: 0x%x\n", OesdkGdtInformationGet()->TaskStateSelector);
```

[Back to the API index](./README.md)
