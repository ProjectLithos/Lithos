# OESDK 0.17.10

- Replaced the obsolete `manifest.example.json` from the 0.0.11 ProjectLithos/Lithos release cycle.
- The example now mirrors the current `source-ready`, `complete-script-only` manifest schema and uses local `ReleaseAssets` paths.
- Added verification that rejects legacy repository URLs, legacy URL-only package entries, stale versions, and missing current-schema fields.
- Updated `Tools/BuildPackages.py` to read the SDK version from `VERSION` and write generated publish metadata to `ReleaseAssets/manifest.release.json` instead of overwriting the shipped manifest and example.
