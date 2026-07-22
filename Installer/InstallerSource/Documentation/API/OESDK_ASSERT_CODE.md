# `OESDK_ASSERT_CODE`

**Category:** Panic and Assertions  
**Kind:** macro  
**Header:** `<oesdk/panic.h>`

Panics with a caller-supplied error code when an expression is false.

## Declaration

```c
#define OESDK_ASSERT_CODE(Expression, ErrorCode)
```

## Parameters

### `Expression`

Condition that must be true.

### `ErrorCode`

Diagnostic code reported on failure.

## Returns

Nothing when true; does not return when false.

## Example

```c
OESDK_ASSERT_CODE(page_aligned, OESDK_STATUS_INVALID_ARGUMENT);
```

[Back to the API index](./README.md)
