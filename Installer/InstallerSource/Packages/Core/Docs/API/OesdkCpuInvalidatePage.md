# `OesdkCpuInvalidatePage`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Invalidates one virtual-address translation on the current CPU.

## Declaration

```c
void OesdkCpuInvalidatePage(uintptr_t VirtualAddress);
```

## Parameters

### `VirtualAddress`

Virtual address whose cached translation must be invalidated.

## Returns

Nothing.

## Notes

- Multiprocessor kernels must also perform the required TLB shootdown on other CPUs.

## Example

```c
OesdkCpuInvalidatePage(address);
```

[Back to the API index](./README.md)
