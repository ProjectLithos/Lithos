# OESDK Online Installer 0.0.3

This is the small Windows bootstrap installer for OESDK. It does not require a
separate OESDK download domain. The package manifest is fetched directly from
the ProjectLithos GitHub repository:

```text
https://raw.githubusercontent.com/ProjectLithos/Lithos/main/Installer/manifest.json
```

Large SDK, architecture, QEMU and Visual Studio integration ZIP files should be
published as assets on the GitHub release named `v0.0.3`. This keeps the setup
executable small and avoids storing large binaries in the Git repository.

## Current behaviour

1. Requests administrator privileges through the normal Windows UAC dialog.
2. Downloads the real `Installer/manifest.json` file from GitHub.
3. Stops with a clear message when the SDK release assets are not published.
4. Once the manifest is marked `ready`, detects complete Visual Studio
   Community, Professional and Enterprise installations.
5. If no full Visual Studio edition exists, downloads Microsoft's validly
   signed Visual Studio Community bootstrapper.
6. Adds the native C/C++ and Clang/LLVM components to the selected edition.
7. Downloads only the packages listed by the manifest.
8. Verifies every package using SHA-256 before extraction.
9. Rejects package destinations that escape the staging directory.
10. Installs the SDK to `C:\OESDK` by default and sets machine-wide
    `OESDK_ROOT`.
11. Installs the OESDK VSIX when the manifest specifies one.
12. Deletes temporary downloads and writes failures to
    `%TEMP%\OESDK-Setup.log`.

The checked-in `manifest.json` intentionally has `releaseStatus` set to
`not-published`. This prevents the installer from downloading Visual Studio or
claiming success before the actual SDK packages exist.

## Publishing the SDK packages

1. Build these four ZIP files:

   - `OESDK-Core-0.0.3.zip`
   - `OESDK-x86_64-0.0.3.zip`
   - `OESDK-QEMU-x86_64.zip`
   - `OESDK-VisualStudio-0.0.3.zip`

2. Create GitHub release `v0.0.3` in `ProjectLithos/Lithos` and upload them as
   release assets.
3. Calculate each file's SHA-256 value on Windows:

   ```powershell
   Get-FileHash -Algorithm SHA256 .\OESDK-Core-0.0.3.zip
   ```

4. Copy `manifest.example.json` to `manifest.json`, replace every placeholder
   hash with its real 64-character value, and run `updater.bat` again.

## Files

- `OESDK-Setup-0.0.3-x64.exe` — small Windows x64 online installer.
- `OESDK-Setup-0.0.3-x64.sha256` — installer checksum.
- `manifest.json` — stable GitHub manifest endpoint and publication state.
- `manifest.example.json` — completed release manifest template.
- `Source/Installer.ps1` — installer interface and download logic.
- `Source/BuildBootstrapper.py` — deterministic native PE launcher builder.
- `Tests/VerifyBootstrapper.py` — static bootstrapper verification.

## Security status

Package contents are SHA-256 verified. The bootstrapper is not Authenticode
signed because no OESDK code-signing certificate has been supplied. Windows
will therefore identify this build as coming from an unknown publisher.

The production manifest should additionally be digitally signed. SHA-256
protects packages against accidental or malicious modification relative to the
manifest, but does not by itself authenticate the manifest publisher.

## Rebuilding

Run with Python 3 from the installer directory:

```text
python Source/BuildBootstrapper.py
```

The builder creates a native Windows PE executable which launches the embedded
installer interface with administrator rights. It does not bundle the SDK
payload.
