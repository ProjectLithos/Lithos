# `OesdkLibcInitialize`

**Category:** LibC Host  
**Kind:** function  
**Header:** `<oesdk/libc.h>`

Initializes the OESDK libc integration state.

## Declaration

```c
void OesdkLibcInitialize(void);
```

## Returns

Nothing.

## Notes

- Install required host callbacks before using libc operations that depend on kernel services.

## Example

```c
OesdkLibcInitialize();
```

[Back to the API index](./README.md)
