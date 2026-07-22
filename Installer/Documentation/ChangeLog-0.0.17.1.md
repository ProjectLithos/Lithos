# OESDK 0.0.17.1

- Added a kernel-owned 256-entry x86-64 IDT.
- Added assembly entry stubs for architectural exception vectors 0-31.
- Added a common register frame and fatal exception dispatcher.
- Assigned the existing TSS IST stacks to double fault, NMI, and machine check.
- Added public IDT information and exception-name APIs.
- Updated the kernel build to compile every x86-64 `.S` architecture source.
- Kept the updater's own file version independent from the SDK package version.
