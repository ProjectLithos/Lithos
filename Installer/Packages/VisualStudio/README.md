# OESDK Visual Studio integration 0.0.7

The VSIX supplies exactly two current templates:

- **OESDK 0.0.7 - Clang C Kernel (no .NET)**
- **OESDK 0.0.7 - Clang C Desktop OS (no .NET)**

Both create native `.vcxproj` Makefile projects backed by the OESDK PowerShell
build system, Clang C, LLD and QEMU. Neither creates a `.csproj` or requires the
Microsoft .NET SDK.

`C:\OESDK\Tools\New-OESDKProject.bat` can create the same native project
without using Visual Studio's project-template cache.
