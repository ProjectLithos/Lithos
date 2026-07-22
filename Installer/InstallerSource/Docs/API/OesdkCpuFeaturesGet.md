# `OesdkCpuFeaturesGet`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Returns the cached CPU feature information.

## Declaration

```c
const OesdkCpuFeatures *OesdkCpuFeaturesGet(void);
```

## Returns

Pointer to the read-only `OesdkCpuFeatures` record.

## Notes

- Call after CPU initialization; the standard runtime has already done so before `kmain`.

## Example

```c
if (OesdkCpuFeaturesGet()->Nx) { kputs("NX supported\n"); }
```

[Back to the API index](./README.md)
