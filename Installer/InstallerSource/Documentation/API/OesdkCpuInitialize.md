# `OesdkCpuInitialize`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Queries the processor and initializes the cached CPU feature record.

## Declaration

```c
bool OesdkCpuInitialize(void);
```

## Returns

`true` when required CPU initialization succeeds; otherwise `false`.

## Notes

- The runtime calls this before `kmain`.

## Example

```c
OESDK_ASSERT(OesdkCpuInitialize());
```

[Back to the API index](./README.md)
