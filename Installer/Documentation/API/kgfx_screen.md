# `kgfx_screen`

**Category:** Graphics  
**Kind:** function  
**Header:** `<oesdk/graphics.h>`

Returns the current framebuffer surface description.

## Declaration

```c
const kgfx_surface *kgfx_screen(void);
```

## Returns

Pointer to a read-only `kgfx_surface`, or `NULL` when unavailable.

## Notes

- Check `kgfx_available` or the returned pointer before dereferencing it.

## Example

```c
const kgfx_surface *screen = kgfx_screen();
```

[Back to the API index](./README.md)
