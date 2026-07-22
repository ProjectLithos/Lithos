# OESDK 0.0.16.1

## Added

- Public `OesdkPanic` and `OesdkPanicAt` fatal-error APIs.
- `OESDK_PANIC`, `OESDK_ASSERT`, and `OESDK_ASSERT_CODE` macros.
- Fatal reporting over COM1 in both Debug and Release configurations.
- VGA text-console panic mirroring.
- Panic recursion protection.
- Source file, function, line, reason, module and 64-bit error reporting.

## Changed

- Boot Context initialization failure now uses the common panic subsystem.
- OESDK revision macros and package metadata now identify 0.0.16.1.
