# `OESDK_PANIC`

**Category:** Panic and Assertions  
**Kind:** macro  
**Header:** `<oesdk/panic.h>`

Invokes `OesdkPanicAt` with the current source location.

## Declaration

```c
#define OESDK_PANIC(Module, Reason, ErrorCode)
```

## Parameters

### `Module`

Subsystem name.

### `Reason`

Failure description.

### `ErrorCode`

64-bit-compatible diagnostic code.

## Returns

Does not return.

## Example

```c
OESDK_PANIC("Scheduler", "No runnable idle thread", 0x2001);
```

[Back to the API index](./README.md)
