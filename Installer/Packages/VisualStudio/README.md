# OESDK Visual Studio integration 0.0.11

The installer supplies exactly two current templates:

- **OESDK 0.0.11 - Clang C Kernel (no .NET)**
- **OESDK 0.0.11 - Clang C Desktop OS (no .NET)**

Both create native `.vcxproj` Makefile projects backed by the OESDK PowerShell
build system, Clang C, LLD and QEMU. Neither creates a `.csproj` or requires the
Microsoft .NET SDK. Setup places their ZIP files directly in Visual Studio
2022's per-user project-template directory and rebuilds the catalogue. The VSIX
is retained in the package only as a development artifact and is not required
for discovery.

`C:\OESDK\Tools\New-OESDKProject.bat` can create the same native project
without using Visual Studio's project-template cache.
