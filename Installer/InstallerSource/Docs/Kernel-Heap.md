# Kernel Heap

OESDK 0.17.21 introduces the bootstrap bump allocator. It consumes a caller-supplied contiguous virtual range and never frees allocations.

`Result = AlignUp(Current, Alignment)` and `Next = Result + RequestedSize`. Allocation succeeds only when the alignment is a non-zero power of two, arithmetic does not overflow, and `Next <= HeapEnd`.

Use `OesdkHeapBootstrapInitialize`, `OesdkHeapBootstrapAllocate`, and `OesdkHeapBootstrapAllocateZeroed`. The permanent free-list heap will replace this bootstrap stage in a later revision.
