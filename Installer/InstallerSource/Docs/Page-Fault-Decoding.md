# Page-Fault Decoding

OESDK 0.17.13 decodes the x86-64 page-fault error code and captures the faulting virtual address from CR2.

`OesdkPageFaultDecode(Address, ErrorCode)` decodes a supplied address and error code, which is suitable for tests and saved fault records. `OesdkPageFaultDecodeCurrent(ErrorCode)` reads CR2 immediately and should be used by the live page-fault handler before any code can disturb diagnostic state.

## Error-code fields

| Bit | Public mask | Meaning when set | Meaning when clear |
|---:|---|---|---|
| 0 | `OESDK_PAGE_FAULT_PRESENT_BIT` | Protection violation | Non-present page |
| 1 | `OESDK_PAGE_FAULT_WRITE_BIT` | Write access | Read access |
| 2 | `OESDK_PAGE_FAULT_USER_BIT` | User-mode access | Supervisor access |
| 3 | `OESDK_PAGE_FAULT_RESERVED_BIT` | Reserved page-table bit was set | No reserved-bit violation |
| 4 | `OESDK_PAGE_FAULT_INSTRUCTION_BIT` | Instruction fetch | Data access |
| 5 | `OESDK_PAGE_FAULT_PROTECTION_KEY_BIT` | Protection-key violation | No protection-key violation |
| 6 | `OESDK_PAGE_FAULT_SHADOW_STACK_BIT` | Shadow-stack access | Not a shadow-stack access |
| 15 | `OESDK_PAGE_FAULT_SGX_BIT` | SGX-related violation | Not SGX-related |

The decoded `OesdkPageFaultInformation` keeps the raw address and error code alongside one Boolean field for every architectural condition.
