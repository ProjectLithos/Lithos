# OESDK 0.17.5

- Added a kernel-owned x86-64 GDT.
- Added kernel and user code/data descriptors.
- Added a 64-bit TSS and task-register loading.
- Added dedicated 16 KiB IST stacks for double fault, NMI, and machine check.
- Added `OesdkGdtInitialize`, `OesdkGdtInformationGet`, and `OesdkGdtSetKernelStack`.
- Initialized the GDT before Boot Context and user `kmain`.
- Updated package revision and public headers to 0.17.5.
- Expanded the freestanding `stdint.h` contract with standard integer constant macros, integer limits, maximum-width types, and pointer-width limits.
