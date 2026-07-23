# Timer

OESDK provides a common timer API with PIT, Local APIC, x2APIC and HPET backends. Hardware interrupt routing and end-of-interrupt delivery remain the responsibility of the interrupt-controller module.

## PIT

The PIT input clock is `1,193,182 Hz`. The divisor is calculated as:

```c
Divisor = (1193182U + DesiredHz / 2U) / DesiredHz;
ActualHz = 1193182U / Divisor;
```

## Tick conversion

Conversions divide first to avoid overflowing on large tick counts:

```c
Seconds = Ticks / Hz;
Remainder = Ticks % Hz;
Nanoseconds = Seconds * 1000000000ULL
            + Remainder * 1000000000ULL / Hz;
```

`OesdkTimerTicksToMilliseconds` uses the same decomposition. Invalid zero frequencies return zero, and saturated results return `UINT64_MAX`.

## APIC, x2APIC and HPET

Local APIC initialization accepts the mapped APIC base, initial count and divide configuration. x2APIC uses its timer MSRs when CPUID reports x2APIC support. HPET initialization accepts a mapped HPET base, reads the counter period, verifies periodic capability and programs timer 0.
