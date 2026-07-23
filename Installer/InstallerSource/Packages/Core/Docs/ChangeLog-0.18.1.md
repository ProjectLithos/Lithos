# OESDK 0.18.1

- Added BSP and AP role discovery using firmware-provided APIC IDs.
- Added per-CPU startup records, kernel-stack assignment, per-CPU pointers, and online counts.
- Added xAPIC and x2APIC INIT/SIPI delivery.
- Added low-memory AP trampoline preparation contract.
- Added bounded AP readiness handshake and failure/timeout reporting.
- Preserved safe single-CPU fallback when no AP becomes online.
- SDK identity is `0.18.1`; package identity is `0.18.1.0`.
