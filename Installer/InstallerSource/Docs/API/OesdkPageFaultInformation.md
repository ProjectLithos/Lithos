# `OesdkPageFaultInformation`

**Category:** Interrupts  
**Kind:** type  
**Header:** `<oesdk/interrupt.h>`

Contains a decoded x86-64 page-fault address and error code.

## Declaration

```c
typedef struct OesdkPageFaultInformation
```

## Fields

- `Address` — faulting virtual address, normally read from CR2.
- `ErrorCode` — original CPU page-fault error code.
- `PresentViolation` — protection violation when set; non-present page when clear.
- `WriteAccess` — write access when set; read access when clear.
- `UserAccess` — user-mode access when set; supervisor access when clear.
- `ReservedBitViolation` — reserved page-table bit was set.
- `InstructionFetch` — fault occurred during instruction fetch.
- `ProtectionKeyViolation` — protection-key restriction caused the fault.
- `ShadowStackAccess` — shadow-stack access caused the fault.
- `SgxViolation` — SGX-related violation.

## Example

```c
OesdkPageFaultInformation fault =
    OesdkPageFaultDecode(OesdkCpuReadCr2(), frame->ErrorCode);
```

[Back to the API index](./README.md)
