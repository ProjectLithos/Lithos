# Kernel Heap — Bootstrap Bump Allocator

OESDK 0.17.22 provides the first kernel-heap stage: a deterministic bootstrap bump allocator. It is intended for early kernel construction, before the permanent free-list heap is available.

## Allocation formula

```c
Result = AlignUp(Current, Alignment);
Next   = Result + RequestedSize;
```

The allocation succeeds only when all of the following are true:

```c
Alignment != 0
Alignment is a power of two
RequestedSize > 0
Next >= Result
Next <= HeapEnd
```

`Next >= Result` ensures that `Result + RequestedSize` did not wrap around the address space. OESDK checks the addition before changing heap state.

After a successful allocation, `Current` becomes `Next`. Failed allocations leave the heap unchanged. Alignment padding is counted as used heap space, because the bump pointer cannot move backwards.

## Public operations

- `OesdkHeapBootstrapInitialize` establishes `[Base, End)` and resets all statistics.
- `OesdkHeapBootstrapAllocate` returns aligned, uninitialised storage.
- `OesdkHeapBootstrapAllocateZeroed` performs overflow-safe `Count × Size` calculation and clears the allocation.
- `OesdkHeapBootstrapIsInitialized` reports whether a heap has been configured.
- `OesdkHeapInformationGet` exposes capacity, usage, remaining bytes, current pointer, end pointer, and allocation count.

## Lifetime limitation

A bump allocator cannot free individual allocations. This is intentional: it is small, predictable, and suitable while constructing page tables, object metadata, and the later permanent heap. The free-list allocator will replace it for general kernel allocation in a later revision.
