# `OesdkPanic`

**Category:** Panic and Assertions  
**Kind:** function  
**Header:** `<oesdk/panic.h>`

Reports a fatal kernel error and halts execution.

## Declaration

```c
OESDK_NORETURN void OesdkPanic(const char *Module, const char *Reason, uint64_t ErrorCode);
```

## Parameters

### `Module`

Subsystem reporting the failure.

### `Reason`

Readable failure description.

### `ErrorCode`

Subsystem-defined 64-bit diagnostic code.

## Returns

Does not return.

## Notes

- Writes fatal diagnostics through the panic output path and includes recursion protection.

## Example

```c
OesdkPanic("Memory", "Allocator metadata corrupt", 0x1001);
```

[Back to the API index](./README.md)
