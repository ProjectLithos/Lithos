# OESDK Complete 0.0.17.1

This is a script-only complete SDK package.

It contains the full installer source, OESDK source and headers, Newlib libc
integration, Visual Studio project templates, `Update.bat`, and `Setup.bat`.

No custom `.exe` file is included. `Setup.bat` invokes the local PowerShell
script, which uses the Microsoft Visual Studio Installer already installed on
the computer. When Visual Studio is absent, it opens Microsoft's official
download page rather than downloading an unsigned OESDK executable.
