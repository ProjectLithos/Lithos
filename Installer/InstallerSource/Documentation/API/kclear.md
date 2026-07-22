# `kclear`

**Category:** Console  
**Kind:** function  
**Header:** `<oesdk/console.h>`

Clears the kernel text console.

## Declaration

```c
void kclear(void);
```

## Returns

Nothing.

## Notes

- This affects the text console; it does not clear the graphics framebuffer.

## Example

```c
kclear();
```

[Back to the API index](./README.md)
