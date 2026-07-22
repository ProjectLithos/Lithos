# `OesdkCpuHalt`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Executes one x86 HLT instruction.

## Declaration

```c
void OesdkCpuHalt(void);
```

## Returns

Nothing after the CPU resumes.

## Notes

- With interrupts disabled, the CPU may remain halted until an NMI, reset, or platform event.

## Example

```c
OesdkCpuHalt();
```

[Back to the API index](./README.md)
