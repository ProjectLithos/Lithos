# `OesdkInterruptFrameDump`

**Category:** Interrupts  
**Kind:** function  
**Header:** `<oesdk/interrupt.h>`

Writes the saved exception register frame to both the serial and kernel consoles.

## Declaration

```c
void OesdkInterruptFrameDump(const OesdkInterruptFrame *Frame);
```

## Notes

A `NULL` frame is ignored. This routine is intended for fatal exception diagnostics and debugger bring-up.

[Back to the API index](./README.md)
