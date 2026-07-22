# OESDK 0.0.16.3

## Kernel types and status codes

- Defined `OesdkStatus` as `int32_t`.
- Defined the initial ABI-stable status values from `OESDK_STATUS_SUCCESS` (`0`) through `OESDK_STATUS_CORRUPT` (`-8`).
- Added `OESDK_SUCCEEDED` and `OESDK_FAILED`.
- Added status name and description helpers for diagnostics.
- Added an automated status-contract verifier.
- Kept source, Core package, and installer-source copies synchronized.
