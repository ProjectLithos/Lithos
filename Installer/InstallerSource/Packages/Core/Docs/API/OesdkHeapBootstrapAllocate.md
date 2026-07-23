# OesdkHeapBootstrapAllocate

```c
void *OesdkHeapBootstrapAllocate(size_t Size, size_t Alignment);
```

Allocates `Size` bytes from the bootstrap heap with the requested power-of-two alignment.

The implementation follows:

```c
Result = AlignUp(Current, Alignment);
Next   = Result + Size;
```

The call succeeds only when `Next >= Result` and `Next <= HeapEnd`. A failure returns `NULL` and does not modify the heap state. The bootstrap allocator cannot free individual allocations.
