# OESDK 0.17.16

## Base template coverage

- Expanded the base kernel template to demonstrate every core SDK subsystem
  currently linked into generated kernels.
- Added boot-context, CPU, GDT/TSS, IDT, status, interrupt registration,
  memory-map normalisation, console, debug, graphics, panic/assertion and CPU
  interrupt-state examples.
- Kept exception triggering, hardware-interrupt enabling and optional Newlib
  initialization out of automatic startup because they require project policy.
- Updated the command-line project generator and every Visual Studio template
  mirror to emit the same complete base source.
- Added a regression test that checks template mirrors and public API coverage.
