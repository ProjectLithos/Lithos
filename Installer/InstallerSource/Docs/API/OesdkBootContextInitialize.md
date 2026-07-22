# `OesdkBootContextInitialize`

**Category:** Boot Context  
**Kind:** function  
**Header:** `<oesdk/boot_context.h>`

Validates the bootloader handoff and initializes the global boot context.

## Declaration

```c
bool OesdkBootContextInitialize(uint32_t BootMagic, uintptr_t BootInformationAddress);
```

## Parameters

### `BootMagic`

Boot protocol magic supplied by the bootstrap entry code.

### `BootInformationAddress`

Physical address of the bootloader information structure.

## Returns

`true` when the context was initialized; otherwise `false`.

## Notes

- The OESDK runtime calls this before `kmain`. Kernel code normally reads the context rather than calling this again.

## Example

```c
if (!OesdkBootContextInitialize(magic, info_address)) {
    OESDK_PANIC("Boot", "Invalid boot context", 1);
}
```

[Back to the API index](./README.md)
