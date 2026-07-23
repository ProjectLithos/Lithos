# OesdkAllocateAligned

```c
void *OesdkAllocateAligned(size_t Size, size_t Alignment);
```

Allocates storage at a power-of-two alignment. Valid prefix and suffix ranges are retained as free blocks; unusably small fragments are absorbed or skipped.
