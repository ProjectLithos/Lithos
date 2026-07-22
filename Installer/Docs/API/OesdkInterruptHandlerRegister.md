# `OesdkInterruptHandlerRegister`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Registers one handler and context pointer for an IDT vector.

## Declaration

```c
OesdkStatus OesdkInterruptHandlerRegister(
    uint8_t Vector,
    OesdkInterruptHandler Handler,
    void *Context);
```

## Returns

- `OESDK_STATUS_SUCCESS` when installed.
- `OESDK_STATUS_NOT_FOUND` when the IDT has not been initialized.
- `OESDK_STATUS_INVALID_ARGUMENT` when `Handler` is `NULL`.
- `OESDK_STATUS_ALREADY_EXISTS` when the vector already has a handler.

## Notes

Registration preserves and restores the caller's interrupt-enable state. The later PIC/APIC module remains responsible for routing IRQs and sending end-of-interrupt notifications.

[Back to the API index](./README.md)
