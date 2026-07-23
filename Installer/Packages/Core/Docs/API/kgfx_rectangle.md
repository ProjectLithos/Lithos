# `kgfx_rectangle`

**Category:** Graphics  
**Kind:** function  
**Header:** `<oesdk/graphics.h>`

Fills a rectangular framebuffer region.

## Declaration

```c
void kgfx_rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t rgb);
```

## Parameters

### `x`

Left coordinate.

### `y`

Top coordinate.

### `width`

Rectangle width in pixels.

### `height`

Rectangle height in pixels.

### `rgb`

Colour encoded as `0xRRGGBB`.

## Returns

Nothing.

## Notes

- The implementation clips drawing to the screen bounds.

## Example

```c
kgfx_rectangle(20, 20, 200, 100, 0x336699);
```

[Back to the API index](./README.md)
