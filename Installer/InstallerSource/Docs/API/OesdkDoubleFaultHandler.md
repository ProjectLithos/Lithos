# `OesdkDoubleFaultHandler`

```c
void OesdkDoubleFaultHandler(OesdkInterruptFrame *Frame);
```

Handles vector 8 on the dedicated double-fault IST stack and terminates safely.

`Frame` must point to the normalised frame created by the OESDK x86-64 exception stub.
