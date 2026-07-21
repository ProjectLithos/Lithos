# OESDK QEMU integration

`Tools/Ensure-Qemu.ps1` installs QEMU through Windows Package Manager when it
is absent, then records the executable path in the machine-wide `OESDK_QEMU`
environment variable. `Tools/Run-Qemu.ps1` launches the compiled kernel and
routes COM1 output to the host console.
