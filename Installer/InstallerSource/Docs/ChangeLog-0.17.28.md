# OESDK 0.17.28

- Fixed runtime IDT initialization to interpret `OesdkIdtInitialize()` as an `OesdkStatus`.
- Successful status value `0` no longer triggers the IDT panic with error `0x13`.
- Added a regression test preventing status-returning initializers from being checked as booleans.
