# OESDK Installer Source 0.0.16

This update adds a complete Newlib-based libc and libm integration for OESDK.

It contains:

- versioned OESDK libc host ABI;
- Newlib syscall and locking adapters;
- verified Newlib 4.6.0.20260123 source bootstrap;
- Debug and Release builds;
- symbol package generator;
- smoke and compile-surface tests;
- Windows/WSL installation and build entry points.

The Newlib source archive is intentionally downloaded from the official
Sourceware distribution and verified against its SHA-256 before extraction.
It is not silently replaced by a partial home-grown libc.
