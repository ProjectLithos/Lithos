# `OesdkStatus`

**Category:** Status  
**Kind:** type  
**Header:** `<oesdk/status.h>`

Defines the signed status type used by recoverable OESDK operations.

## Declaration

```c
typedef int32_t OesdkStatus
```

## Returns

A signed 32-bit status value.

## Notes

- Zero and positive values indicate success; negative values indicate failure.

## Example

```c
OesdkStatus status = OESDK_STATUS_SUCCESS;
```

[Back to the API index](./README.md)
