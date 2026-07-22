# `OesdkCpuRegisters`

**Category:** CPU  
**Kind:** type  
**Header:** `<oesdk/cpu.h>`

Holds the four general-purpose register values returned by a CPUID query.

## Declaration

```c
typedef struct OesdkCpuRegisters
```

## Returns

A structure filled by `OesdkCpuQuery`.

## Example

```c
OesdkCpuRegisters registers;
OesdkCpuQuery(0, 0, &registers);
```

[Back to the API index](./README.md)
