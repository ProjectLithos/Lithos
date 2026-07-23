# OESDK CPU Primitives and Feature Detection

OESDK 0.0.16.2 adds the first kernel-owned x86-64 CPU abstraction. It provides
small architectural primitives and a cached CPUID feature record without
forcing a particular kernel architecture.

## Startup

`oesdk_runtime_start()` initializes CPU feature detection before the user's
`kmain()` function. Failure to confirm x86-64 long-mode support enters the common
panic path.

```c
const OesdkCpuFeatures *Features = OesdkCpuFeaturesGet();
kprintf("CPU vendor: %s\\n", Features->Vendor);
```

## Detected capabilities

The feature record reports long mode, NX, APIC, x2APIC, SYSCALL/SYSRET, TSC,
invariant TSC and 1 GiB page support. It also retains the maximum supported
basic and extended CPUID leaves.

A raw `OesdkCpuQuery()` operation is available for kernel code that needs a
CPUID leaf not represented by the initial feature structure.

## Interrupt state

`OesdkCpuInterruptStateSave()` returns RFLAGS and disables maskable interrupts.
Pass that exact value to `OesdkCpuInterruptStateRestore()` to restore the prior
interrupt-enabled state. This avoids accidentally enabling interrupts when a
caller entered a critical section with interrupts already disabled.

## Privileged operations

Control-register access, interrupt control, `HLT`, and `INVLPG` are privileged
kernel operations. They must not be called by ring-3 code. `OesdkCpuHalt()`
executes one halt instruction and can return after an interrupt; permanent
fatal halting remains the responsibility of the panic subsystem.
