# IDT and Exception Stubs

OESDK 0.17.12 provides a kernel-owned x86-64 IDT with all 256 entries and architectural exception stubs for vectors 0 through 31.

The assembly entry layer preserves the general-purpose registers, normalises CPU error-code and non-error-code exceptions into one `OesdkInterruptFrame`, clears the direction flag, aligns the stack for the C dispatcher, and returns with `iretq` when a registered handler completes.

## Exception frame

`OesdkInterruptFrame` contains R15 through RAX, the vector and error code, RIP, CS, RFLAGS, RSP, and SS. RSP and SS are CPU-supplied when an exception changes privilege level or uses an Interrupt Stack Table entry.

## Installed exception vectors

All vectors 0–31 are installed. CPU-supplied error codes are preserved for vectors 8, 10, 11, 12, 13, 14, 17, 21, 29, and 30; all other exception stubs supply an error code of zero.

Dedicated first handlers are provided for divide error, invalid opcode, general protection fault, page fault, double fault, and machine check. Unhandled architectural exceptions use the common fatal diagnostic path. Page faults additionally report CR2 and decode the architectural error-code fields.

Double fault uses IST1, NMI uses IST2, and machine check uses IST3 from the complete GDT/TSS module.
