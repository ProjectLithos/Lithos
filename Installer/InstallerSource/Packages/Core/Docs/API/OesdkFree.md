# OesdkFree

```c
void OesdkFree(void *Address);
```

Releases a valid allocation and coalesces physically contiguous free neighbours. `NULL`, out-of-range pointers, invalid headers, and double frees are ignored.
