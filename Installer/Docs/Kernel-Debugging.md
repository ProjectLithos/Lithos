# Running and debugging an OESDK kernel

`powershell.exe` is not the debugger. It is the scripting host used to locate
and launch QEMU and GDB.

## Run

```powershell
C:\OESDK\Tools\Run-Kernel.ps1 -Kernel .\Build\Debug\kernel.elf
```

## Debug

```powershell
C:\OESDK\Tools\Debug-Kernel.ps1 -Kernel .\Build\Debug\kernel.elf -BreakAt kmain
```

The debug command starts QEMU paused with its GDB server on TCP port 1234,
loads the ELF symbols in GDB, connects to the guest, sets the requested
breakpoint and continues execution.

Set `OESDK_GDB` to the full path of `x86_64-elf-gdb.exe` when GDB is not on
PATH. The serial output is written beside the kernel as `serial-debug.log`.

The misleading Visual Studio Local Windows Debugger configuration has been
removed because that debugger would debug the Windows launcher or QEMU host
process, not the guest kernel.
