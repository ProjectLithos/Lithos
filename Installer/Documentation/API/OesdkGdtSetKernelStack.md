# `OesdkGdtSetKernelStack`

**Category:** GDT and TSS  
**Kind:** function  
**Header:** `<oesdk/gdt.h>`

Updates the TSS ring-0 stack pointer used for privilege transitions.

## Declaration

```c
bool OesdkGdtSetKernelStack(uintptr_t StackTop);
```

## Parameters

### `StackTop`

Top of the new, valid ring-0 stack.

## Returns

`true` when accepted; otherwise `false`.

## Notes

- Call when switching the active process or thread if each has a distinct kernel stack.

## Example

```c
OESDK_ASSERT(OesdkGdtSetKernelStack(thread->KernelStackTop));
```

[Back to the API index](./README.md)
