# `OesdkBootProtocol`

**Category:** Boot Context  
**Kind:** type  
**Header:** `<oesdk/boot_context.h>`

Identifies the boot protocol used to enter the kernel.

## Declaration

```c
typedef enum OesdkBootProtocol
```

## Returns

An enumeration value stored in `OesdkBootContext.BootProtocol`.

## Notes

- Currently defines `OesdkBootProtocolUnknown` and `OesdkBootProtocolMultiboot1`.

## Example

```c
OesdkBootProtocol protocol = OesdkBootContextGet()->BootProtocol;
```

[Back to the API index](./README.md)
