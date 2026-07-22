# OESDK 0.17.6

## Updater ZIP selection correction

- Updated `Update.bat` to discover both canonical `OESDK-Z.Y.X.zip` packages and legacy `OESDK-Complete-Z.Y.X.zip` packages.
- Candidate packages are still opened and validated before selection.
- Selection uses the package version from `manifest.json` or `VERSION`, then the file timestamp only as a tie-breaker.
- Bumped the updater implementation version to `0.0.17.3`.
