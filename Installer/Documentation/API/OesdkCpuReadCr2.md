# `OesdkCpuReadCr2`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Reads the page-fault linear address from CR2.

## Declaration

```c
uint64_t OesdkCpuReadCr2(void);
```

## Returns

Current CR2 value.

## Notes

- Most useful from a page-fault handler.

## Example

```c
uint64_t fault_address = OesdkCpuReadCr2();
```

[Back to the API index](./README.md)
