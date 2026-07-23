# OESDK 0.17.23

- Added the permanent doubly linked free-list kernel heap.
- Added exact block metadata with magic, payload size, free state, and neighbour links.
- Added split handling with a minimum reusable block size.
- Added forward and backward coalescing of physically contiguous blocks.
- Added `OesdkAllocate`, aligned allocation, overflow-safe zeroed allocation, and `OesdkFree`.
- Retained the bootstrap bump allocator for early kernel construction.
- Updated the base template, API documentation, package mirrors, and regression tests.
