# `OesdkPageFaultDecode`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Decodes an x86-64 page-fault address and CPU error code without performing I/O or halting.

## Declaration

```c
OesdkPageFaultInformation OesdkPageFaultDecode(
    uintptr_t Address,
    uint64_t ErrorCode);
```

## Returns

An `OesdkPageFaultInformation` value containing the raw inputs and decoded bits 0-6 and 15.

## Example

```c
OesdkPageFaultInformation fault =
    OesdkPageFaultDecode(OesdkCpuReadCr2(), frame->ErrorCode);
```

[Back to the API index](./README.md)
