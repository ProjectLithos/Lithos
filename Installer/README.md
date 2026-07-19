# OESDK Online Installer 0.0.1

This is the first OESDK Windows bootstrapper. The executable contains the
installation interface and download logic, but it does not contain the SDK,
LLVM, QEMU, project templates, or Visual Studio extension.

## Current behaviour

1. Requests administrator privileges through the normal Windows UAC dialog.
2. Downloads an HTTPS package manifest.
3. Downloads only the packages listed by that manifest.
4. verifies every package using SHA-256 before extraction.
5. Rejects package destinations that escape the staging directory.
6. Installs the selected SDK files to `C:\OESDK` by default.
7. Sets the machine-wide `OESDK_ROOT` environment variable.
8. Downloads Microsoft's validly signed Visual Studio Community bootstrapper
   when Community is absent, or uses the installed Visual Studio Installer.
9. Adds the native C/C++ and Clang/LLVM components to Visual Studio Community.
10. Installs the downloaded OESDK VSIX when the manifest specifies one.
11. Deletes temporary package downloads after installation.

The stable manifest address shown by the installer is reserved but is not yet
published. Until a package host is available, enter the HTTPS address of a
manifest matching `manifest.example.json`.

## Files

- `OESDK-Setup-0.0.1-x64.exe` — small Windows x64 online installer.
- `OESDK-Setup-0.0.1-x64.sha256` — installer checksum.
- `manifest.example.json` — package-server manifest contract.
- `Source/Installer.ps1` — installer interface and package installation logic.
- `Source/BuildBootstrapper.py` — deterministic native PE launcher builder.

## Security status

Package contents are SHA-256 verified. The bootstrapper is not Authenticode
signed because no OESDK code-signing certificate has been supplied. Windows
will therefore identify this first build as coming from an unknown publisher.

The production manifest should additionally be digitally signed. That is the
next security milestone; SHA-256 alone proves integrity against the manifest
but does not authenticate who published the manifest.

## Rebuilding

Run with Python 3 from the installer directory:

```text
python Source/BuildBootstrapper.py
```

The builder creates a native Windows PE executable which launches the embedded
installer interface with administrator rights. It does not rename a script to
an `.exe` file and it does not bundle the SDK payload.
