# `OesdkCpuReadCr3`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Reads the current page-table root from CR3.

## Declaration

```c
uint64_t OesdkCpuReadCr3(void);
```

## Returns

Current CR3 value.

## Notes

- Low bits may contain architecture-defined flags or a PCID when enabled.

## Example

```c
uint64_t page_table_root = OesdkCpuReadCr3();
```

[Back to the API index](./README.md)
