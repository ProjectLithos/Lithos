# OESDK Online Installer and SDK Packages 0.0.5

This source tree builds the small OESDK Windows bootstrapper and the first
downloadable SDK package set. The installer reads its manifest directly from:

```text
https://raw.githubusercontent.com/ProjectLithos/Lithos/main/Installer/manifest.json
```

## Included packages

- **Core**: freestanding C headers, a small libc, VGA console, formatting,
  DEBUG-only COM1 output, runtime startup, and framebuffer drawing primitives.
- **x86-64**: QEMU-compatible Multiboot entry point, 64-bit paging, linker script,
  `Build-Kernel.ps1`, and `Run-Qemu.ps1`.
- **QEMU**: automatic QEMU installation through Windows Package Manager and
  machine-wide `OESDK_QEMU` configuration.
- **Visual Studio**: a VSIX containing **OESDK Freestanding Kernel** and
  **OESDK Desktop GUI OS** project templates.

The generated package archives live in `ReleaseAssets`. They are downloaded
separately, keeping `OESDK-Setup-0.0.5-x64.exe` small.

## User experience

1. Run the setup executable and approve Windows UAC.
2. The installer downloads and SHA-256 verifies the four packages.
3. If no full Visual Studio edition exists, it downloads Visual Studio 2022
   Community from Microsoft.
4. It adds the native C/C++ and Clang/LLVM components.
5. It installs QEMU through `winget` when QEMU is absent.
6. It installs the OESDK VSIX.
7. In Visual Studio, select an OESDK template, build it, and use **Start Without
   Debugging** to launch the resulting `kernel.elf` in QEMU.

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
