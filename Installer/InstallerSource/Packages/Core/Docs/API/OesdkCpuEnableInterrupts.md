# `OesdkCpuEnableInterrupts`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Sets the interrupt-enable flag for the current CPU.

## Declaration

```c
void OesdkCpuEnableInterrupts(void);
```

## Returns

Nothing.

## Notes

- Do not enable interrupts before a valid IDT and interrupt-controller setup exist.

## Example

```c
OesdkCpuEnableInterrupts();
```

[Back to the API index](./README.md)
