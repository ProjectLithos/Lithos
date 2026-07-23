# OesdkAllocate

```c
void *OesdkAllocate(size_t Size);
```

Allocates naturally aligned kernel storage from the permanent free-list heap. Returns `NULL` for zero size, an uninitialized heap, or insufficient space.
