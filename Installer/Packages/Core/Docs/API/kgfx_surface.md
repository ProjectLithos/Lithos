# `kgfx_surface`

**Category:** Graphics  
**Kind:** type  
**Header:** `<oesdk/graphics.h>`

Describes a linear graphics surface exposed by the boot environment.

## Declaration

```c
typedef struct kgfx_surface
```

## Returns

A structure returned by `kgfx_screen`.

## Notes

- Pixel encoding depends on the current framebuffer setup; current helpers accept 24-bit RGB values.

## Example

```c
const kgfx_surface *screen = kgfx_screen();
```

[Back to the API index](./README.md)
