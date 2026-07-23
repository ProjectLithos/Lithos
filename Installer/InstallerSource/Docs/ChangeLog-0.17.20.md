# OESDK 0.17.21

- Formalised the public virtual mapping, unmapping, and translation interfaces.
- Changed the mapping parameter to the requested public `uint64_t Flags` type.
- Added a complete public flag mask and rejection of unknown bits.
- Added explicit multiplication and address-addition overflow checks before page-table changes.
- Expanded API documentation and regression coverage for the mapping contract.
