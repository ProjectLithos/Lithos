# `OESDK_SUCCEEDED`

**Category:** Status  
**Kind:** macro  
**Header:** `<oesdk/status.h>`

Tests whether an OESDK status indicates success.

## Declaration

```c
#define OESDK_SUCCEEDED(Status)
```

## Parameters

### `Status`

Value convertible to `OesdkStatus`.

## Returns

Non-zero when the status is zero or positive; otherwise zero.

## Example

```c
if (OESDK_SUCCEEDED(status)) { /* continue */ }
```

[Back to the API index](./README.md)
