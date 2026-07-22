# `OesdkBootContextGet`

**Category:** Boot Context  
**Kind:** function  
**Header:** `<oesdk/boot_context.h>`

Returns the initialized, read-only boot context.

## Declaration

```c
const OesdkBootContext *OesdkBootContextGet(void);
```

## Returns

Pointer to the global `OesdkBootContext`.

## Notes

- The runtime initializes the context before calling `kmain`.

## Example

```c
const OesdkBootContext *context = OesdkBootContextGet();
```

[Back to the API index](./README.md)
