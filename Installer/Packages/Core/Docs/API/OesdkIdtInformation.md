# `OesdkIdtInformation`

**Category:** Interrupts  
**Kind:** type  
**Header:** `<oesdk/interrupt.h>`

Describes the active interrupt descriptor table.

## Declaration

```c
typedef struct OesdkIdtInformation
```

## Returns

A read-only structure returned by `OesdkIdtInformationGet`.

## Example

```c
const OesdkIdtInformation *idt = OesdkIdtInformationGet();
```

[Back to the API index](./README.md)
