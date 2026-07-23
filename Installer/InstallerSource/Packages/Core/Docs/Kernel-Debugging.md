
## Normal run versus debugging

Visual Studio F5 and Start Without Debugging launch `Run-Kernel.cmd`. This starts QEMU normally and does not start or connect GDB.

To debug the guest kernel, run `Debug-Kernel.cmd` explicitly. That separate command starts QEMU paused with its GDB server and opens the interactive GDB console.
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


## Interactive GDB console

F5 opens a separate console titled:

```text
OESDK Kernel Debugger - GDB
```

The console remains open and accepts normal GDB commands. Common commands
include:

```text
continue
break <symbol>
stepi
nexti
info registers
x/16gx <address>
backtrace
disassemble
```

Closing GDB ends the QEMU debugging session.

## Command reference

See [Condensed GDB Command Reference](GDB-Command-Reference.md).

## Visual Studio toolbar buttons

OESDK generated projects use `Tools/VisualStudio-Launch.ps1` as the Visual Studio launch command.

- **F5 / filled green triangle**: Visual Studio starts the launcher under its native debugger. The launcher detects this and runs `Debug-Kernel.ps1`, which starts QEMU paused and attaches GDB to the guest kernel.
- **Ctrl+F5 / Start Without Debugging / outlined triangle**: the launcher is not itself debugged, so it runs `Run-Kernel.ps1` and starts QEMU normally without GDB.

The text `Local Windows Debugger` is Visual Studio's built-in debugger-flavour label. It does not mean the kernel is a Windows program.


## Visual Studio launch completion

The Visual Studio wrapper starts the QEMU and GDB session asynchronously. It returns success after both processes have started; closing GDB with Ctrl+C or the window close button is treated as normal session termination rather than an SDK launch failure. The generated GDB session closes its matching QEMU process when the debugger window exits.
