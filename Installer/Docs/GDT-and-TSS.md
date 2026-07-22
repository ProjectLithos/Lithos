# Kernel-owned GDT and TSS

OESDK 0.0.17.0 replaces the temporary bootstrap descriptor table with a kernel-owned x86-64 Global Descriptor Table.

The table retains the existing kernel selectors, adds ring-3 code and data descriptors, and installs a 64-bit Task State Segment. Three dedicated 16 KiB Interrupt Stack Table stacks are reserved for double fault, NMI, and machine check handling.

## Selectors

- `0x08` kernel code
- `0x10` kernel data
- `0x1B` user data with RPL 3
- `0x23` user code with RPL 3
- `0x28` 64-bit TSS

`OesdkGdtSetKernelStack()` updates TSS `RSP0` before a ring-3 transition. Its address must be non-zero and 16-byte aligned.

`OesdkGdtInformationGet()` exposes read-only diagnostic information, including the GDT base, limit, TSS selector, ring-0 stack, and IST stack tops.
