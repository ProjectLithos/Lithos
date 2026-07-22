# `oesdk_debug_printf`

**Category:** Debug  
**Kind:** function  
**Header:** `<oesdk/debug.h>`

Writes formatted debugging output in Debug builds.

## Declaration

```c
int oesdk_debug_printf(const char *format, ...);
```

## Parameters

### `format`

OESDK-supported format string.

### `...`

Values referenced by the format string.

## Returns

Number of characters written, or a negative value on failure.

## Notes

- Prefer `kdebugf` in ordinary kernel code so Release builds compile the call away.

## Example

```c
oesdk_debug_printf("thread=%u\n", thread_id);
```

[Back to the API index](./README.md)
