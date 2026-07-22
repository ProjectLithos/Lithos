# `OesdkStatusDescription`

**Category:** Status  
**Kind:** function  
**Header:** `<oesdk/status.h>`

Returns a readable description of a common OESDK status.

## Declaration

```c
const char *OesdkStatusDescription(OesdkStatus Status);
```

## Parameters

### `Status`

Status value to describe.

## Returns

Pointer to a static description string.

## Notes

- The returned string must not be freed or modified.

## Example

```c
kprintf("Error: %s\n", OesdkStatusDescription(status));
```

[Back to the API index](./README.md)
