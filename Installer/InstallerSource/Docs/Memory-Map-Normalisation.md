# Memory-map normalisation

OESDK converts Multiboot 1 memory-map records into an architecture-independent array of `OesdkMemoryRegion` values.

Every region is represented as the half-open interval `[Base, Base + Length)`. A region is accepted only when `Length > 0` and the end calculation does not overflow. Two valid regions overlap when `A.Base < B.End && B.Base < A.End`.

The normaliser validates every variable-size Multiboot record, converts bootloader types, sorts regions by base address, rejects ambiguous overlaps, and coalesces adjacent regions of the same type. The fixed initial capacity is 256 regions.

Multiboot types map as follows: available to `OesdkMemoryAvailable`, ACPI reclaimable to `OesdkMemoryAcpiReclaimable`, ACPI NVS to `OesdkMemoryAcpiNvs`, bad RAM to `OesdkMemoryBad`, and unknown or reserved values to `OesdkMemoryReserved`. Kernel, boot-data, and framebuffer classifications are available for later reservation overlays.
