# OESDK Visual Studio integration 0.0.12

The installer supplies exactly two current templates:

- **OESDK 0.0.12 - Clang C Kernel (no .NET)**
- **OESDK 0.0.12 - Clang C Desktop OS (no .NET)**

Both create native `.vcxproj` Makefile projects backed by the OESDK PowerShell
build system, Clang C, LLD and QEMU. Neither creates a `.csproj` or requires the
Microsoft .NET SDK. Setup places their ZIP files directly in Visual Studio
2022's per-user project-template directory and rebuilds the catalogue. The VSIX
is retained in the package only as a development artifact and is not required
for discovery.

`C:\OESDK\Tools\New-OESDKProject.bat` can create the same native project
without using Visual Studio's project-template cache.

During setup, the user supplies an author name, email address, OS licence and
initial OS version. The installed templates are personalised with those values.
Source/header comments contain their independent file version rather than the
OS version. Each project also receives `OESDKProject.json`,
`OESDKFileVersions.json`, `AUTHORS`, `LICENSE`, and `Include/OSVersion.h`.
