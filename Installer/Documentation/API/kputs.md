# `kputs`

**Category:** Console  
**Kind:** function  
**Header:** `<oesdk/console.h>`

Writes a null-terminated string to the kernel console.

## Declaration

```c
void kputs(const char *text);
```

## Parameters

### `text`

Null-terminated text to write.

## Returns

Nothing.

## Notes

- Pass a valid non-null string.

## Example

```c
kputs("Kernel started.\n");
```

[Back to the API index](./README.md)
