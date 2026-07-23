# Kernel Heap — Free-list Allocator

OESDK 0.17.23 retains the bootstrap bump allocator and adds the permanent free-list heap. The bump allocator remains suitable while the permanent allocator is being constructed; after `OesdkHeapInitialize`, general kernel code uses `OesdkAllocate`, `OesdkAllocateAligned`, `OesdkAllocateZeroed`, and `OesdkFree`.

## Block layout

```c
typedef struct OesdkHeapBlock
{
    uint64_t Magic;
    size_t Size;
    bool Free;
    struct OesdkHeapBlock *Previous;
    struct OesdkHeapBlock *Next;
} OesdkHeapBlock;
```

The magic value detects invalid headers. The doubly linked list permits constant-time neighbour access during coalescing.

## Splitting

A free block is split only when:

```c
OriginalSize >= RequestedSize
              + HeaderSize
              + MinimumBlockSize
```

Small unusable suffixes are absorbed into the allocation. Aligned allocation may also create a valid prefix free block; prefixes too small to hold a header and minimum payload are skipped by advancing to the next aligned address.

## Coalescing

Free neighbours are merged only when physically contiguous:

```c
Address(Block) + HeaderSize + Block.Size
    == Address(Block.Next)
```

`OesdkFree` coalesces forward and backward, rebuilding larger reusable ranges and reducing fragmentation. Invalid pointers, double frees, and pointers outside the heap are ignored safely.

## Public operations

- `OesdkHeapInitialize` creates one initial free block over caller-supplied memory.
- `OesdkAllocate` uses the natural pointer alignment.
- `OesdkAllocateAligned` requires a non-zero power-of-two alignment.
- `OesdkAllocateZeroed` rejects `Count × Size` overflow before allocation.
- `OesdkFree` releases and coalesces an allocation.
- `OesdkHeapInformationGet` reports capacity, used/free bytes, total and active allocations, free blocks, and the largest free block.

## Zeroed allocation overflow

```c
if (Count != 0 && Size > SIZE_MAX / Count)
    return NULL;
```

The freestanding implementation expresses `SIZE_MAX` portably as `((size_t)-1)`.

## Bootstrap allocator

The original bump formula remains available before permanent-heap initialization:

```c
Result = AlignUp(Current, Alignment);
Next   = Result + RequestedSize;
```

It still cannot free individual allocations. Allocation succeeds only when:

```c
Next >= Result
Next <= HeapEnd
```
