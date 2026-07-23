# `OesdkMachineCheckHandler`

```c
void OesdkMachineCheckHandler(OesdkInterruptFrame *Frame);
```

Handles vector 18 on the dedicated machine-check IST stack and terminates safely.

`Frame` must point to the normalised frame created by the OESDK x86-64 exception stub.
