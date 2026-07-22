# `OesdkLibcGetHost`

**Category:** LibC Host  
**Kind:** function  
**Header:** `<oesdk/libc.h>`

Returns the currently installed libc host callback table.

## Declaration

```c
const OesdkLibcHost *OesdkLibcGetHost(void);
```

## Returns

Installed host pointer, or `NULL` if none is installed.

## Example

```c
const OesdkLibcHost *host = OesdkLibcGetHost();
```

[Back to the API index](./README.md)
