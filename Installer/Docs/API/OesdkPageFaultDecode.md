# OesdkPageFaultDecode

```c
OesdkPageFaultInformation OesdkPageFaultDecode(uintptr_t Address, uint64_t ErrorCode);
```

Decodes a supplied page-fault address and x86-64 error code into `OesdkPageFaultInformation`. This form is deterministic and suitable for tests, stored fault records, and offline diagnostics.
