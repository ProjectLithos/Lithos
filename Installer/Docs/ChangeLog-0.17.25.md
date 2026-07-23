# OESDK 0.17.28

- Fixed the base kernel template so Visual Studio IntelliSense can parse `TemplateMultibootInformation`.
- Removed the unnecessary GNU `__attribute__((packed))` from that structure; all fields are naturally 32-bit aligned and the required Multiboot layout is unchanged.
- Updated all Visual Studio, project-generator, package, and installer-source template copies.
- Added a regression check preventing the non-portable declaration from returning.
