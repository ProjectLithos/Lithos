# OESDK 0.17.7

- Completed the x86-64 Exceptions and Interrupts foundation.
- Installed valid IDT gates for all 256 vectors.
- Added all architectural exception stubs and correct error-code handling.
- Added reusable interrupt handler registration.
- Added page-fault decoding using CR2 and the architectural error bits.
- Added full register-frame diagnostics before fatal exception panic.
- Preserved dedicated IST stacks for double fault, NMI and machine check.
- Updated the API reference for every new public interrupt type and function.
