# OESDK 0.17.19

## x86-64 page-table manager

- Exposed the 48-bit PML4, PDPT, PD, PT and offset layout through named public constants.
- Added `OesdkVirtualAddressParts` and `OesdkVirtualAddressDecode`.
- Added explicit lower- and upper-canonical range constants.
- Added the future higher-half kernel base `0xFFFFFFFF80000000` without relocating the running kernel.
- Refactored mapper table lookup and translation to use the shared decoder.
- Expanded API documentation and regression validation.
