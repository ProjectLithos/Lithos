# `OesdkPageFaultHandler`

```c
void OesdkPageFaultHandler(OesdkInterruptFrame *Frame);
```

Handles vector 14, reads CR2, decodes the page-fault error code, dumps registers, and panics.

`Frame` must point to the normalised frame created by the OESDK x86-64 exception stub.
