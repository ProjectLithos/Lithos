# `OesdkInterruptHandlerUnregister`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Removes a previously registered vector handler.

## Declaration

```c
OesdkStatus OesdkInterruptHandlerUnregister(
    uint8_t Vector,
    OesdkInterruptHandler Handler);
```

## Returns

- `OESDK_STATUS_SUCCESS` when removed.
- `OESDK_STATUS_NOT_FOUND` when the IDT is unavailable or the supplied handler is not registered for the vector.
- `OESDK_STATUS_INVALID_ARGUMENT` when `Handler` is `NULL`.

[Back to the API index](./README.md)
