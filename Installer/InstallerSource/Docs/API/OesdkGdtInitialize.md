# `OesdkGdtInitialize`

**Category:** GDT and TSS  
**Kind:** function  
**Header:** `<oesdk/gdt.h>`

Builds and loads the kernel-owned GDT and 64-bit TSS.

## Declaration

```c
bool OesdkGdtInitialize(void);
```

## Returns

`true` on success; otherwise `false`.

## Notes

- The runtime calls this before IDT initialization and before `kmain`.

## Example

```c
OESDK_ASSERT(OesdkGdtInitialize());
```

[Back to the API index](./README.md)
