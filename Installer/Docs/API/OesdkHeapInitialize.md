# OesdkHeapInitialize

```c
OesdkStatus OesdkHeapInitialize(void *Base, size_t Size);
```

Initializes the permanent free-list heap over `[Base, Base + Size)`. The start is naturally aligned and one initial free block is created. Returns `OESDK_STATUS_INVALID_ARGUMENT` for invalid or overflowing ranges and `OESDK_STATUS_OUT_OF_MEMORY` when the range cannot hold a header plus the minimum block.
