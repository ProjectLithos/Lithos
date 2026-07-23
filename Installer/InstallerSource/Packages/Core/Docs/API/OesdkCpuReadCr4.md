# `OesdkCpuReadCr4`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Reads control register CR4.

## Declaration

```c
uint64_t OesdkCpuReadCr4(void);
```

## Returns

Current CR4 value.

## Notes

- Privileged operation; kernel mode only.

## Example

```c
uint64_t cr4 = OesdkCpuReadCr4();
```

[Back to the API index](./README.md)
