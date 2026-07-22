# `kgfx_fill`

**Category:** Graphics  
**Kind:** function  
**Header:** `<oesdk/graphics.h>`

Fills the entire framebuffer with one colour.

## Declaration

```c
void kgfx_fill(uint32_t rgb);
```

## Parameters

### `rgb`

Colour encoded as `0xRRGGBB`.

## Returns

Nothing.

## Example

```c
kgfx_fill(0x102030);
```

[Back to the API index](./README.md)
