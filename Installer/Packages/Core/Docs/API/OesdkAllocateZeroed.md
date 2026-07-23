# OesdkAllocateZeroed

```c
void *OesdkAllocateZeroed(size_t Count, size_t Size);
```

Checks `Count × Size` for overflow, allocates the total size, and clears every byte. Returns `NULL` on overflow or allocation failure.
