# OESDK 0.17.17

- Added the 4 KiB bitmap physical page-frame allocator.
- Added overflow-safe alignment and page/address conversion helpers.
- Permanently reserves page zero, kernel/BSS, bootstrap stack, bootstrap page tables, Multiboot structures, framebuffer, and allocator bitmap storage.
- Added contiguous allocation, freeing, permanent-page protection, statistics, API documentation, base-template integration, and regression tests.
