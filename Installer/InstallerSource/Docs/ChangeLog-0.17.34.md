# OESDK 0.17.34

- Fixed Visual Studio F5 sessions being reported as failed after the user closed or interrupted GDB.
- `Debug-Kernel.ps1` now returns after starting the interactive GDB console instead of waiting for the debugger session to end.
- The generated GDB session treats Windows Ctrl+C exit code `-1073741510` as normal user termination and cleans up its matching QEMU process.
- Persistent debugger and Visual Studio launcher logs still record process identifiers and startup failures.
