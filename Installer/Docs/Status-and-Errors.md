# OESDK Status and Error Contract

OESDK module 0.0.16.3 establishes one error convention for all kernel modules.

`OesdkStatus` is a signed 32-bit integer. Zero and positive values are successful; negative values are failures. Callers use `OESDK_SUCCEEDED` and `OESDK_FAILED` rather than mixing Boolean results, arbitrary negative integers, and module-specific conventions.

The initial ABI-stable values are success, invalid argument, not supported, out of memory, not found, already exists, busy, timeout, and corrupt. Their numeric values are fixed from `0` through `-8`.
