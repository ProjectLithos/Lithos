# `OesdkCpuDisableInterrupts`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Clears the interrupt-enable flag for the current CPU.

## Declaration

```c
void OesdkCpuDisableInterrupts(void);
```

## Returns

Nothing.

## Notes

- Use save/restore when the previous interrupt state must be preserved.

## Example

```c
OesdkCpuDisableInterrupts();
```

[Back to the API index](./README.md)
