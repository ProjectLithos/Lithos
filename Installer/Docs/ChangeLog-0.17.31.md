# OESDK 0.17.32

## Visual Studio toolbar launch behaviour

- F5 / the filled green Start button now starts QEMU paused and launches the OESDK GDB debugger.
- Ctrl+F5 / Start Without Debugging now starts QEMU normally without GDB.
- Added `Tools/VisualStudio-Launch.ps1`, which detects whether Visual Studio created the launcher under its native debugger and selects the correct kernel launcher.
- Updated the Kernel and Desktop Visual Studio templates and the command-line project generator.
- The toolbar may continue to display Visual Studio's generic `Local Windows Debugger` label; the selected action is now OESDK-aware.
