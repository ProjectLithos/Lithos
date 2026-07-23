# OESDK 0.17.23

## Bootstrap bump allocator contract

- Formalised allocation as `Result = AlignUp(Current, Alignment)` and `Next = Result + RequestedSize`.
- Added an explicit aligned-address helper with power-of-two and wraparound validation.
- Required `Next >= Result` and `Next <= HeapEnd` before committing an allocation.
- Confirmed that failed allocations leave heap state unchanged.
- Updated heap API documentation and regression checks across all package mirrors.
