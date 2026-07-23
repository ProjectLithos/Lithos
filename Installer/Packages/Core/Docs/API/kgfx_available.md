# `kgfx_available`

**Category:** Graphics  
**Kind:** function  
**Header:** `<oesdk/graphics.h>`

Reports whether a usable graphics framebuffer is available.

## Declaration

```c
bool kgfx_available(void);
```

## Returns

`true` when graphics operations are available; otherwise `false`.

## Example

```c
if (kgfx_available()) { kgfx_fill(0x000000); }
```

[Back to the API index](./README.md)
