# IDT and Exceptions

OESDK 0.0.17.1 installs a kernel-owned x86-64 Interrupt Descriptor Table after the GDT and TSS are active.

The SDK installs exception vectors 0 through 31. NMI uses IST2, double fault uses IST1, and machine check uses IST3. The remaining exceptions use the current kernel stack. Every exception enters a common assembly frame and is reported through the OESDK panic subsystem.

Public API:

```c
bool OesdkIdtInitialize(void);
const OesdkIdtInformation *OesdkIdtInformationGet(void);
const char *OesdkExceptionName(uint8_t Vector);
```

The generated Debug `kernel.elf` retains the individual exception-stub symbols for GDB inspection.
