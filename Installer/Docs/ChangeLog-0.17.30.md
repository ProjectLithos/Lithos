# OESDK 0.17.31

- Separated normal kernel running from guest debugging.
- Visual Studio F5 and Start Without Debugging now launch `Run-Kernel.cmd` and never start GDB.
- `Debug-Kernel.cmd` remains the explicit entry point for QEMU GDB debugging.
- Updated template, manifest, documentation, installer-source, and package mirrors.
