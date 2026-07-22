# OESDK 0.17.9

- Replaced the obsolete `0.0.11` package verifier with validation for the current `source-ready` manifest and package layout.
- Made bootstrapper verification conditional on `containsCustomExecutable`.
- Added validation for the `complete-script-only` setup path through `Setup.bat` and `Tools/Install-VisualStudio.ps1`.
- Kept native PE/bootstrapper verification available for future manifests that declare a custom executable.
- Mirrored both corrected tests into `InstallerSource/Tests`.
