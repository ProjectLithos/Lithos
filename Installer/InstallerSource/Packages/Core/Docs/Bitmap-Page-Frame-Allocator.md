# Bitmap page-frame allocator

OESDK uses 4 KiB physical pages and a bitmap where one bit represents one page frame. A set allocation bit means unavailable or allocated; a clear bit means free.

Initialization consumes the normalized memory map. It exposes only complete page frames from `OesdkMemoryAvailable` regions and permanently reserves page zero, the complete kernel image and BSS, bootstrap stack, bootstrap page tables, Multiboot structures, the Multiboot memory-map storage, framebuffer storage, and the allocator bitmaps.

The allocator supports contiguous allocation and rejects attempts to free permanent pages. Its initial compile-time coverage is 16 GiB (`OESDK_PHYSICAL_MAX_PAGES` frames); later virtual-memory work can replace the static bitmap storage without changing the public allocation contract.
