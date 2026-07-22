# `OesdkCpuQuery`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Executes the x86 CPUID instruction for a leaf and subleaf.

## Declaration

```c
void OesdkCpuQuery(uint32_t Leaf, uint32_t Subleaf, OesdkCpuRegisters *Registers);
```

## Parameters

### `Leaf`

CPUID leaf placed in EAX.

### `Subleaf`

CPUID subleaf placed in ECX.

### `Registers`

Output structure receiving EAX, EBX, ECX and EDX.

## Returns

Nothing.

## Notes

- The output pointer must be valid. Check supported maximum leaves before querying optional leaves.

## Example

```c
OesdkCpuRegisters registers;
OesdkCpuQuery(7, 0, &registers);
```

[Back to the API index](./README.md)
