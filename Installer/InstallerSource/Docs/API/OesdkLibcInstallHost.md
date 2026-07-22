# `OesdkLibcInstallHost`

**Category:** LibC Host  
**Kind:** function  
**Header:** `<oesdk/libc.h>`

Installs the kernel callback table used by OESDK Newlib.

## Declaration

```c
int OesdkLibcInstallHost(const OesdkLibcHost *Host);
```

## Parameters

### `Host`

Pointer to a persistent, ABI-compatible callback table.

## Returns

`0` on success; a non-zero value on failure.

## Notes

- The callback table and any referenced context must remain valid while libc can use them.

## Example

```c
if (OesdkLibcInstallHost(&host) != 0) { OESDK_PANIC("LibC", "Host install failed", 1); }
```

[Back to the API index](./README.md)
