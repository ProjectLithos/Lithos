# OESDK 0.0.16.2

## CPU primitives and CPUID

- Added the public `oesdk/cpu.h` kernel CPU interface.
- Added cached CPUID detection for long mode, NX, APIC, x2APIC,
  SYSCALL/SYSRET, TSC, invariant TSC, and 1 GiB pages.
- Added the CPU vendor string and maximum basic/extended CPUID leaves.
- Added raw CPUID leaf/subleaf querying.
- Added interrupt enable/disable and save/restore helpers.
- Added RFLAGS, CR0, CR2, CR3, and CR4 readers.
- Added CR3 writing, page invalidation, CPU pause, and single-instruction halt.
- CPU detection now runs before Boot Context initialization and before `kmain()`.
- Missing long-mode capability now enters the OESDK panic path.
- Updated package revision and public revision macros to 0.0.16.2.
