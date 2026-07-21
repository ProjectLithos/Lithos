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


## Visual Studio F5

For an NMake C++ project, Visual Studio still labels the toolbar launcher
"Local Windows Debugger". OESDK overrides its command so that F5 runs
`Debug-Kernel.cmd` instead of trying to execute `kernel.elf`.

The Windows debugger only launches the command wrapper. QEMU runs the guest and
GDB performs the actual kernel debugging. The ELF is never executed as a Win32
program.


## F5 launcher diagnostics

The Visual Studio F5 command now uses:

```text
cmd.exe /D /C call "<ProjectDir>\Debug-Kernel.cmd"
```

This avoids the nested quote parsing that could cause `cmd.exe` to exit before
the launcher ran. The launcher writes:

```text
Build\Debug\oesdk-launcher.log
```

The guest debugger writes:

```text
Build\Debug\oesdk-debug.log
```


## GNU debugger installation

OESDK uses GDB for QEMU guest debugging. Setup installs MSYS2 and the
`mingw-w64-ucrt-x86_64-gdb-multiarch` package. The expected debugger is:

```text
C:\msys64\ucrt64\bin\gdb-multiarch.exe
```

Visual Studio's LLDB executable is no longer used as an automatic fallback.
