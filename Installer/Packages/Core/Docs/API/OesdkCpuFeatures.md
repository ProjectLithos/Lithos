# `OesdkCpuFeatures`

**Category:** CPU  
**Kind:** type  
**Header:** `<oesdk/cpu.h>`

Contains CPU vendor information and detected x86-64 capabilities.

## Declaration

```c
typedef struct OesdkCpuFeatures
```

## Returns

A read-only structure returned by `OesdkCpuFeaturesGet`.

## Notes

- The vendor string is null-terminated in the 13-byte `Vendor` field.

## Example

```c
const OesdkCpuFeatures *features = OesdkCpuFeaturesGet();
```

[Back to the API index](./README.md)
