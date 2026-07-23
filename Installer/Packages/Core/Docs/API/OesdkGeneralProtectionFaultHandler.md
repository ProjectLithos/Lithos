# `OesdkGeneralProtectionFaultHandler`

```c
void OesdkGeneralProtectionFaultHandler(OesdkInterruptFrame *Frame);
```

Handles vector 13, preserving and reporting the CPU error code before panic.

`Frame` must point to the normalised frame created by the OESDK x86-64 exception stub.
