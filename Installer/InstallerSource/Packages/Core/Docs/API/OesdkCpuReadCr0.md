# `OesdkCpuReadCr0`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Reads control register CR0.

## Declaration

```c
uint64_t OesdkCpuReadCr0(void);
```

## Returns

Current CR0 value.

## Notes

- Privileged operation; kernel mode only.

## Example

```c
uint64_t cr0 = OesdkCpuReadCr0();
```

[Back to the API index](./README.md)
