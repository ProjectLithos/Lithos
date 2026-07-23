# `OesdkStatusName`

**Category:** Status  
**Kind:** function  
**Header:** `<oesdk/status.h>`

Returns the symbolic name of a common OESDK status.

## Declaration

```c
const char *OesdkStatusName(OesdkStatus Status);
```

## Parameters

### `Status`

Status value to describe.

## Returns

Pointer to a static symbolic-name string.

## Notes

- Unknown values return a stable fallback name.

## Example

```c
kputs(OesdkStatusName(OESDK_STATUS_TIMEOUT));
```

[Back to the API index](./README.md)
