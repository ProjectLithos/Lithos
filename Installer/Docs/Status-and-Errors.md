# OESDK Status and Error Contract

OESDK 0.0.16.3 introduces a single signed 32-bit status type for kernel and SDK APIs.

## Contract

`OesdkStatus` is an `int32_t`. Zero means success and negative values describe failures. Public APIs may later use positive values for successful results that carry additional information, so callers should use `OESDK_SUCCEEDED` and `OESDK_FAILED` rather than comparing only with zero.

```c
OesdkStatus Status = SomeOperation();
if (OESDK_FAILED(Status)) {
    kprintf("%s: %s\n", OesdkStatusName(Status), OesdkStatusDescription(Status));
    return Status;
}
```

## Initial values

The first contract defines success, invalid argument, unsupported operation, out of memory, not found, already exists, busy, timeout, corrupt data, access denied, buffer too small, invalid state, I/O error, interruption, overflow and underflow. Existing numeric values are ABI-stable and must not be reassigned.

## Rules

- Return `OESDK_STATUS_SUCCESS` only when the requested operation completed.
- Validate caller input and return `OESDK_STATUS_INVALID_ARGUMENT` before changing state.
- Use `OESDK_STATUS_BUFFER_TOO_SMALL` when retrying with a larger buffer can succeed.
- Use `OESDK_STATUS_INVALID_STATE` when the input is valid but the object lifecycle does not permit the operation.
- Do not return private module-specific numbers through a public API without allocating them in a documented status range.
- Fatal invariants still use the panic subsystem; recoverable failures return `OesdkStatus`.
