# OesdkPageFaultDecodeCurrent

```c
OesdkPageFaultInformation OesdkPageFaultDecodeCurrent(uint64_t ErrorCode);
```

Reads CR2 and decodes the supplied x86-64 page-fault error code. Call this at the beginning of a live page-fault handler so the returned `Address` is the faulting virtual address.
