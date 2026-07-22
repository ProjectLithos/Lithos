# `kprintf`

**Category:** Console  
**Kind:** function  
**Header:** `<oesdk/console.h>`

Writes formatted text to the kernel console.

## Declaration

```c
int kprintf(const char *format, ...);
```

## Parameters

### `format`

OESDK-supported format string.

### `...`

Values referenced by the format string.

## Returns

Number of characters written, or a negative value on failure.

## Notes

- This is a freestanding formatter; do not assume every hosted C `printf` extension is available.

## Example

```c
kprintf("CR3 = 0x%llx\n", OesdkCpuReadCr3());
```

[Back to the API index](./README.md)
