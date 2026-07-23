# `OesdkPanicAt`

**Category:** Panic and Assertions  
**Kind:** function  
**Header:** `<oesdk/panic.h>`

Reports a fatal kernel error with explicit source-location information.

## Declaration

```c
OESDK_NORETURN void OesdkPanicAt(const char *Module, const char *Reason, uint64_t ErrorCode, const char *File, const char *Function, uint32_t Line);
```

## Parameters

### `Module`

Subsystem reporting the failure.

### `Reason`

Readable failure description.

### `ErrorCode`

Subsystem-defined 64-bit diagnostic code.

### `File`

Source filename.

### `Function`

Function name.

### `Line`

Source line number.

## Returns

Does not return.

## Notes

- Ordinary callers should normally use `OESDK_PANIC`, which supplies location fields automatically.

## Example

```c
OesdkPanicAt("VM", "Invalid PTE", 2, __FILE__, __func__, __LINE__);
```

[Back to the API index](./README.md)
