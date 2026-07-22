# Panic and Assertions

OESDK 0.0.16.1 provides a fatal-error path that is available in both Debug and
Release kernels.

## Panic

```c
OesdkPanic("PhysicalMemory", "Invalid page frame", 4U);
```

`OesdkPanic` disables maskable interrupts, writes the report to COM1 first,
mirrors it to the VGA text console, and halts the processor.

Use `OESDK_PANIC` when source location information should be included:

```c
OESDK_PANIC("Scheduler", "No runnable thread", 0x20U);
```

## Assertions

```c
OESDK_ASSERT(PageCount != 0U);
OESDK_ASSERT_CODE(Address < Limit, 0x31U);
```

A failed assertion reports the expression, file, function, line and optional
64-bit error code. Assertions are always active because silently removing
kernel safety checks from Release builds can turn a diagnosable failure into
memory corruption.

The panic path contains recursion protection. If a second panic occurs while
printing the first report, OESDK emits a minimal serial message and halts.
