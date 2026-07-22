# `OesdkCpuInterruptStateRestore`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Restores the interrupt-enable state represented by saved RFLAGS.

## Declaration

```c
void OesdkCpuInterruptStateRestore(uint64_t Flags);
```

## Parameters

### `Flags`

Value returned by `OesdkCpuInterruptStateSave`.

## Returns

Nothing.

## Notes

- Use only with a value captured by the matching save operation.

## Example

```c
OesdkCpuInterruptStateRestore(flags);
```

[Back to the API index](./README.md)
