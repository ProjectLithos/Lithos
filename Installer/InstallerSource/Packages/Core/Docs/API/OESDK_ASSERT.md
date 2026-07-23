# `OESDK_ASSERT`

**Category:** Panic and Assertions  
**Kind:** macro  
**Header:** `<oesdk/panic.h>`

Panics when an expression evaluates false.

## Declaration

```c
#define OESDK_ASSERT(Expression)
```

## Parameters

### `Expression`

Condition that must be true.

## Returns

Nothing when true; does not return when false.

## Notes

- Assertions remain part of the current SDK contract; do not use them for recoverable errors.

## Example

```c
OESDK_ASSERT(pointer != NULL);
```

[Back to the API index](./README.md)
