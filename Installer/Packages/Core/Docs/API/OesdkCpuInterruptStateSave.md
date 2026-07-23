# `OesdkCpuInterruptStateSave`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Saves RFLAGS and disables maskable interrupts atomically for a critical section.

## Declaration

```c
uint64_t OesdkCpuInterruptStateSave(void);
```

## Returns

The previous RFLAGS value for `OesdkCpuInterruptStateRestore`.

## Example

```c
uint64_t flags = OesdkCpuInterruptStateSave();
/* critical section */
OesdkCpuInterruptStateRestore(flags);
```

[Back to the API index](./README.md)
