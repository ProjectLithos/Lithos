# `OesdkInterruptFrame`

**Category:** Interrupts  
**Kind:** type  
**Header:** `<oesdk/interrupt.h>`

Represents the general-purpose registers and CPU state saved by the x86-64 interrupt stubs.

## Declaration

```c
typedef struct OesdkInterruptFrame
```

## Notes

- `Vector` identifies the exception or interrupt vector.
- `ErrorCode` contains the CPU-supplied error code, or zero for vectors that do not supply one.
- `Rip`, `Cs`, and `Rflags` contain the interrupted execution state.
- The frame is passed to registered handlers and fatal exception diagnostics.

[Back to the API index](./README.md)
