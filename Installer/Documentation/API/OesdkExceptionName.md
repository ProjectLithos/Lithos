# `OesdkExceptionName`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Returns a readable name for an architectural exception vector.

## Declaration

```c
const char *OesdkExceptionName(uint8_t Vector);
```

## Parameters

### `Vector`

Exception vector number, normally 0 through 31.

## Returns

Pointer to a static string.

## Notes

- Out-of-range values return the SDK fallback name.

## Example

```c
kputs(OesdkExceptionName(14));
```

[Back to the API index](./README.md)
