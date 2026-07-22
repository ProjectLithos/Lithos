# `OesdkInterruptHandlerIsRegistered`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Reports whether a vector currently has a registered handler.

## Declaration

```c
bool OesdkInterruptHandlerIsRegistered(uint8_t Vector);
```

## Returns

`true` when a handler is registered; otherwise `false`.

[Back to the API index](./README.md)
