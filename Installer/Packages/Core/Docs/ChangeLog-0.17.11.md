# OESDK 0.17.11

## Complete GDT and TSS

- Corrected the public descriptor selectors to represent the actual GDT slots: null `0x00`, kernel code `0x08`, kernel data `0x10`, user data `0x18`, user code `0x20`, and TSS `0x28`.
- Added explicit RPL-3 user selector macros (`0x1B` and `0x23`) for segment-register and privilege-transition use.
- Expanded the owned x86-64 TSS to populate all seven IST entries with independent 16 KiB aligned stacks.
- Retained dedicated IST assignments for double fault, NMI, and machine check.
- Added initialization-state and IST replacement APIs.
- Expanded GDT diagnostic information and API documentation.
- Added `VerifyGdtContract.py` to reject incomplete or stale GDT packages.
