# OESDK 0.17.12

- Added and documented architectural exception stubs for vectors 0–31.
- Completed `OesdkInterruptFrame` with RSP and SS fields.
- Added dedicated handlers for divide error, invalid opcode, general protection fault, page fault, double fault, and machine check.
- Preserved CPU error codes for every architectural exception that supplies one.
- Expanded interrupt contract verification and API documentation.
