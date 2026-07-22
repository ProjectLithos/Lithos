# `OesdkCpuWriteCr3`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Loads a page-table root into CR3.

## Declaration

```c
void OesdkCpuWriteCr3(uint64_t Value);
```

## Parameters

### `Value`

Physical page-table root address and any valid CR3 flags.

## Returns

Nothing.

## Notes

- An invalid value can immediately fault or halt the kernel. Writing CR3 also affects translation caching.

## Example

```c
OesdkCpuWriteCr3(new_root);
```

[Back to the API index](./README.md)
