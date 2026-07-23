# OESDK 0.0.16.0

- Added `OesdkBootContext` ABI version 1.
- Validates the Multiboot 1 magic and information pointer before user kernel entry.
- Exposes boot protocol, raw boot information address, kernel image/load/BSS boundaries, bootstrap stack boundaries, and available memory.
- Parses Multiboot memory-map records with record-size, range, and integer-overflow checks.
- Falls back to Multiboot lower/upper memory fields when no memory map is supplied.
- Initializes Boot Context before console, graphics, and `kmain()`.
- Exported bootstrap stack linker symbols for debugging and later memory reservation.
- Updated public OESDK version macros to 0.0.16.0.
