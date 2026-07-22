# `OesdkInterruptFrame`

**Category:** Interrupts  
**Kind:** type  
**Header:** `<oesdk/interrupt.h>`

Represents the register frame saved by OESDK exception stubs.

## Declaration

```c
typedef struct OesdkInterruptFrame
```

## Returns

A structure passed internally to exception dispatch.

## Notes

- The initial 0.0.17.1 public API exposes the layout for diagnostics; hardware IRQ registration is not yet public.

## Example

```c
const OesdkInterruptFrame *frame = current_frame;
```

[Back to the API index](./README.md)
