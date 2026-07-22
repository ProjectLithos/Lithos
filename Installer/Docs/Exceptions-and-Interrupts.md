# Exceptions and Interrupts

OESDK 0.17.8 installs a kernel-owned x86-64 IDT with all 256 vectors. Architectural exceptions 0-31 use correctly shaped stubs, including CPU-supplied error codes.

## Safety foundation

- Double fault uses IST1.
- NMI uses IST2.
- Machine check uses IST3.
- Unhandled CPU exceptions print the complete register frame and halt through the panic system.
- Page faults report CR2 and decode error-code bits 0-6 and 15.
- Hardware/software interrupt vectors can register one handler and context.
- Handler table changes preserve and restore the caller interrupt-enable state.

The interrupt controller is deliberately separate. PIC/APIC routing and end-of-interrupt handling belong to the later interrupt-controller module.
