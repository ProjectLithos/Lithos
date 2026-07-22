# `OesdkBootContext`

**Category:** Boot Context  
**Kind:** type  
**Header:** `<oesdk/boot_context.h>`

Describes the validated boot handoff and kernel bootstrap boundaries.

## Declaration

```c
typedef struct OesdkBootContext
```

## Returns

A structure populated by `OesdkBootContextInitialize` and returned by `OesdkBootContextGet`.

## Notes

- Treat the returned context as read-only after initialization.
- `StructureSize` and `AbiVersion` permit future-compatible structure growth.

## Example

```c
const OesdkBootContext *context = OesdkBootContextGet();
kprintf("Available memory: %llu\n", context->AvailableMemoryBytes);
```

[Back to the API index](./README.md)
