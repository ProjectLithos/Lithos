# `kgfx_pixel`

**Category:** Graphics  
**Kind:** function  
**Header:** `<oesdk/graphics.h>`

Writes one pixel to the framebuffer.

## Declaration

```c
void kgfx_pixel(uint32_t x, uint32_t y, uint32_t rgb);
```

## Parameters

### `x`

Horizontal pixel coordinate.

### `y`

Vertical pixel coordinate.

### `rgb`

Colour encoded as `0xRRGGBB`.

## Returns

Nothing.

## Notes

- Coordinates outside the surface are ignored by the SDK implementation.

## Example

```c
kgfx_pixel(10, 10, 0xFFFFFF);
```

[Back to the API index](./README.md)
