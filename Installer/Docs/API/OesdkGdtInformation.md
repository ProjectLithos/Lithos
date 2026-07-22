# `OesdkGdtInformation`

**Category:** GDT and TSS  
**Kind:** type  
**Header:** `<oesdk/gdt.h>`

Describes the active GDT, TSS selector, ring-0 stack, and interrupt stacks.

## Declaration

```c
typedef struct OesdkGdtInformation
```

## Returns

A read-only structure returned by `OesdkGdtInformationGet`.

## Example

```c
const OesdkGdtInformation *gdt = OesdkGdtInformationGet();
```

[Back to the API index](./README.md)
