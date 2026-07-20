# OESDK Online Installer and SDK Packages 0.0.12

This source tree builds the small OESDK Windows bootstrapper and the first
downloadable SDK package set. The installer reads its manifest directly from:

```text
https://raw.githubusercontent.com/ProjectLithos/Lithos/main/Installer/manifest.json
```

## Included packages

- **Core**: freestanding C headers, a small libc, VGA console, formatting,
  DEBUG-only COM1 output, runtime startup, and framebuffer drawing primitives.
- **x86-64**: QEMU-compatible Multiboot entry point, 64-bit paging, linker script,
  build/run scripts, a direct native project generator, and a Visual Studio
  template-repair utility.
- **QEMU**: automatic QEMU installation through Windows Package Manager and
  machine-wide `OESDK_QEMU` configuration.
- **Visual Studio**: exactly two versioned native `.vcxproj` templates:
  **OESDK 0.0.12 - Clang C Kernel (no .NET)** and **OESDK 0.0.12 - Clang C
  Desktop OS (no .NET)**. Setup places them directly in Visual Studio 2022's
  per-user project-template directory.

The generated package archives live in `ReleaseAssets`. They are downloaded
separately, keeping `OESDK-Setup-0.0.12-x64.exe` small.

Version 0.0.12 is a clean replacement. After verifying the new downloads, setup
removes the old OESDK installation, all OESDK VSIX registrations, and all old
OESDK template ZIPs. It then installs exactly two new templates directly into
Visual Studio's user-template directory and rebuilds the catalogue. The 0.0.12
templates prevent Windows command-line quoting from corrupting a project path
that ends in a backslash. The build script also repairs that malformed argument
when an existing project was created from the 0.0.8 template.

The freestanding `stddef.h`, `stdint.h`, and `stdbool.h` headers now provide a
Visual Studio IntelliSense branch, while Clang continues to use its native
freestanding types. Desktop-only and debug-only helpers are also compiled only
when their corresponding configuration is selected, keeping the Error List
free from unused-function warnings.

Every generated Visual Studio project now configures the SDK include directory
in both **VC++ Directories / Include Directories** and **NMake / Include Search
Path**. It also defines `__INTELLISENSE__=1` for the code model, with `DEBUG=1`
and `OESDK_DESKTOP=1` added only to the matching project configuration.

Setup now asks for the author's name, email address, OS licence and initial OS
version. It personalises every installed Visual Studio template with those
values. Generated C and header files contain a metadata comment with their own
independent file version; the OS version is deliberately absent from that
comment and is stored in `OESDKProject.json` and `Include/OSVersion.h` instead.
The selected full licence text, `AUTHORS`, and `OESDKFileVersions.json` are also
created with every new project.

## User experience

1. Run the setup executable and approve Windows UAC.
2. Enter the author name, author email, OS licence and initial OS version.
3. The installer downloads and SHA-256 verifies the four packages.
4. If no full Visual Studio edition exists, it downloads Visual Studio 2022
   Community from Microsoft.
5. It adds the native C/C++ and Clang/LLVM components.
6. It installs QEMU through `winget` when QEMU is absent.
7. It removes all previous OESDK SDK files, VSIX registrations, and templates.
8. It installs the two personalised template ZIPs directly into Visual Studio and
   rebuilds its project-template catalogue.
9. In Visual Studio, select one of the two OESDK 0.0.12 native templates, build
   it, and use **Start Without
   Debugging** to launch the resulting `kernel.elf` in QEMU.

To create a project without the Visual Studio template picker:

```bat
C:\OESDK\Tools\New-OESDKProject.bat Koryn Kernel "%USERPROFILE%\source\repos"
```

Then open `Koryn.vcxproj` from the newly created project folder.

To change one tracked source file's version and update both its comment and the
central file-version registry:

```powershell
C:\OESDK\Tools\Set-OESDKFileVersion.ps1 -ProjectRoot . -File Source\kmain.c -Version 0.0.2
```

Builds validate the author, email, licence, SPDX identifier and file version in
every tracked source/header before invoking Clang.

Debug configurations define `DEBUG`, so `kdebugf(...)` is compiled in and sent
to COM1. Release configurations omit it. `kprintf(...)` always targets the VGA
text screen.

## Building everything

From this directory:

```text
python Tools/BuildPackages.py
python Source/BuildBootstrapper.py
python Tests/VerifyBootstrapper.py
```

Run `Tools/BuildPackages.py` once more after editing package sources so the
source ZIP contains the updated release assets and manifest.

## Current limitations

- The runtime is a starter SDK, not a complete ISO/UEFI boot stack.
- The QEMU automation requires Windows Package Manager (`winget`) if QEMU is
  not already installed.
- The installer and VSIX are not Authenticode signed because no Project Lithos
  code-signing certificate has been supplied.
- The manifest is SHA-256 protected but is not yet digitally signed.
