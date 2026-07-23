# `OesdkCpuReadFlags`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Reads the current x86-64 RFLAGS register.

## Declaration

```c
uint64_t OesdkCpuReadFlags(void);
```

## Returns

Current RFLAGS value.

## Example

```c
uint64_t flags = OesdkCpuReadFlags();
```

[Back to the API index](./README.md)
