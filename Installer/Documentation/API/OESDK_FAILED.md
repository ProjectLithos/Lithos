# `OESDK_FAILED`

**Category:** Status  
**Kind:** macro  
**Header:** `<oesdk/status.h>`

Tests whether an OESDK status indicates failure.

## Declaration

```c
#define OESDK_FAILED(Status)
```

## Parameters

### `Status`

Value convertible to `OesdkStatus`.

## Returns

Non-zero when the status is negative; otherwise zero.

## Example

```c
if (OESDK_FAILED(status)) { kputs(OesdkStatusDescription(status)); }
```

[Back to the API index](./README.md)
