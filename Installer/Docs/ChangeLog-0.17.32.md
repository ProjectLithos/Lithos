# OESDK 0.17.32

- Made the Visual Studio launcher persist complete diagnostics in `Build/<Configuration>/oesdk-visualstudio-launch.log`.
- Launcher failures now remain visible until Enter is pressed instead of closing immediately.
- Added validation for the generated kernel, SDK root, and selected run/debug script.
- Hardened quoted path and child-process exit-code handling.
