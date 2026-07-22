# `OesdkDivideErrorHandler`

```c
void OesdkDivideErrorHandler(OesdkInterruptFrame *Frame);
```

Handles vector 0 and terminates through the panic subsystem after dumping the saved frame.

`Frame` must point to the normalised frame created by the OESDK x86-64 exception stub.
