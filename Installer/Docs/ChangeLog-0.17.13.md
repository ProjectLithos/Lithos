# OESDK 0.17.13

- Added named x86-64 page-fault error-code masks for bits 0 through 6 and bit 15.
- Added `OesdkPageFaultDecodeCurrent`, which captures CR2 and decodes the CPU error code.
- Updated the live page-fault handler to use the CR2-capturing helper.
- Added complete page-fault API documentation and regression coverage.
