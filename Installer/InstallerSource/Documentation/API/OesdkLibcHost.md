# `OesdkLibcHost`

**Category:** LibC Host  
**Kind:** type  
**Header:** `<oesdk/libc.h>`

Defines the kernel services that the separately built OESDK Newlib layer calls.

## Declaration

```c
typedef struct OesdkLibcHost
```

## Returns

A host callback table installed with `OesdkLibcInstallHost`.

## Notes

- Set `AbiVersion` to `OESDK_LIBC_ABI_VERSION`. Callbacks may be null when the corresponding service is unsupported, subject to libc adapter behaviour.

## Example

```c
OesdkLibcHost host = {0};
host.AbiVersion = OESDK_LIBC_ABI_VERSION;
```

[Back to the API index](./README.md)
