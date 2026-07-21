# Condensed GDB Command Reference

This guide covers the commands most useful when debugging an OESDK kernel in
QEMU.

Press **F5** in an OESDK Visual Studio project to start QEMU and open the
interactive **OESDK Kernel Debugger - GDB** console.

## Essential session commands

| Command | Explanation |
|---|---|
| `help` | Show GDB help. Use `help <command>` for details about one command. |
| `continue` or `c` | Resume kernel execution until a breakpoint, watchpoint, fault, or interrupt stops it. |
| `interrupt` | Stop a running target and return to the GDB prompt. In the console, `Ctrl+C` normally performs this action. |
| `quit` or `q` | End GDB. OESDK then closes the associated QEMU debugging session. |
| `file Build/Debug/kernel.elf` | Load the kernel executable and debugging symbols. OESDK runs this automatically. |
| `target remote 127.0.0.1:1234` | Connect GDB to QEMU's built-in GDB server. OESDK runs this automatically. |

## Breakpoints

| Command | Explanation |
|---|---|
| `break kmain` or `b kmain` | Stop when `kmain` is reached. |
| `break FunctionName` | Stop when the named function is reached. |
| `break SourceFile.c:42` | Stop at line 42 in the named source file. |
| `break *0x100000` | Stop when execution reaches an exact address. |
| `tbreak FunctionName` | Create a temporary breakpoint that removes itself after it is hit once. |
| `info breakpoints` | List all breakpoints and watchpoints. |
| `disable 2` | Disable breakpoint number 2 without deleting it. |
| `enable 2` | Re-enable breakpoint number 2. |
| `delete 2` | Delete breakpoint number 2. |
| `condition 2 expression` | Stop at breakpoint 2 only when the expression is true. |
| `commands 2` | Attach commands to breakpoint 2. Finish the command list with `end`. |

Example conditional breakpoint:

```gdb
break SchedulerDispatch
condition 1 CurrentThread == 0
```

## Stepping through code

| Command | Explanation |
|---|---|
| `step` or `s` | Execute the next source line, entering called functions. |
| `next` or `n` | Execute the next source line without entering called functions. |
| `stepi` or `si` | Execute one machine instruction, entering calls. |
| `nexti` or `ni` | Execute one machine instruction without entering calls. |
| `finish` | Run until the current function returns. |
| `until` | Run until execution reaches a later source line in the current frame. |
| `advance Location` | Continue until the named source line, function, or address is reached. |

For early boot code and assembly, use `stepi` and `nexti` rather than
source-level `step` and `next`.

## Registers and CPU state

| Command | Explanation |
|---|---|
| `info registers` | Display the general-purpose CPU registers. |
| `info all-registers` | Display all registers known to GDB. |
| `print/x $rax` | Display `RAX` in hexadecimal. |
| `print/d $rax` | Display `RAX` as a signed decimal value. |
| `set $rax = 0` | Change a register value. |
| `print/x $rip` | Display the current instruction pointer. |
| `print/x $rsp` | Display the current stack pointer. |
| `print/x $cr3` | Display CR3 when the selected GDB target exposes control registers. |

Useful x86-64 registers include:

```text
RIP  current instruction address
RSP  current stack pointer
RBP  current frame/base pointer
RAX  return value and general-purpose register
RDI  first integer or pointer argument
RSI  second integer or pointer argument
RDX  third integer or pointer argument
RCX  fourth integer or pointer argument
R8   fifth integer or pointer argument
R9   sixth integer or pointer argument
RFLAGS processor status flags
```

## Variables and expressions

| Command | Explanation |
|---|---|
| `print Variable` or `p Variable` | Display a variable or expression. |
| `print/x Variable` | Display the value in hexadecimal. |
| `print/t Variable` | Display the value in binary. |
| `ptype TypeName` | Show the definition of a type. |
| `whatis Variable` | Show the type of a variable. |
| `display Expression` | Print an expression automatically every time execution stops. |
| `undisplay Number` | Remove an automatic display expression. |
| `set variable Name = Value` | Change a program variable. |
| `info locals` | Show local variables in the current stack frame. |
| `info args` | Show function arguments in the current stack frame. |

Examples:

```gdb
print CurrentThread
print/x PageTableRoot
ptype struct OesdkThread
set variable SchedulerEnabled = 0
```

## Reading and writing memory

The general memory examination form is:

```text
x/<count><format><size> address
```

Common formats:

```text
x  hexadecimal
d  signed decimal
u  unsigned decimal
t  binary
c  character
s  string
i  machine instruction
```

Common sizes:

```text
b  byte
h  halfword (2 bytes)
w  word (4 bytes)
g  giant word (8 bytes)
```

| Command | Explanation |
|---|---|
| `x/16xb Address` | Show 16 bytes in hexadecimal. |
| `x/8xg Address` | Show eight 64-bit values in hexadecimal. |
| `x/10i $rip` | Disassemble the next ten instructions. |
| `x/s Address` | Display a null-terminated string. |
| `x/32cb Address` | Display 32 bytes as characters. |
| `set {unsigned long long}Address = Value` | Write a 64-bit value to memory. |
| `find Start, End, Value` | Search a memory range for a value. |

Examples:

```gdb
x/16xg $rsp
x/20i $rip
x/s MessagePointer
set {unsigned int}0x100000 = 0x12345678
```

Be careful when writing memory. Changing page tables, interrupt structures, or
device registers can immediately halt the kernel.

## Stack traces and frames

| Command | Explanation |
|---|---|
| `backtrace` or `bt` | Show the current call stack. |
| `bt full` | Show the call stack with local variables. |
| `frame 2` | Select stack frame number 2. |
| `up` | Move to the calling frame. |
| `down` | Move toward the innermost frame. |
| `info frame` | Show detailed information about the selected frame. |
| `info locals` | Show local variables in the selected frame. |
| `info args` | Show arguments in the selected frame. |

Kernel stack traces are most reliable when the kernel is built with debugging
symbols and frame pointers.

## Source and disassembly

| Command | Explanation |
|---|---|
| `list` or `l` | Show source lines around the current location. |
| `list FunctionName` | Show source for the named function. |
| `list SourceFile.c:42` | Show source around line 42. |
| `disassemble FunctionName` | Disassemble a function. |
| `disassemble /m FunctionName` | Show mixed source and assembly when supported. |
| `layout src` | Open GDB's text user interface source view. |
| `layout asm` | Open the assembly view. |
| `layout regs` | Show the register view. |
| `layout split` | Show source and assembly together. |
| `tui disable` | Leave the text user interface. |

Useful interactive keys in TUI mode:

```text
Ctrl+X, A   toggle TUI mode
Ctrl+L      redraw the screen
Page Up     scroll the active TUI window
Page Down   scroll the active TUI window
```

## Symbols and addresses

| Command | Explanation |
|---|---|
| `info functions Pattern` | List functions whose names match a pattern. |
| `info variables Pattern` | List variables whose names match a pattern. |
| `info address Symbol` | Show the address of a symbol. |
| `info symbol Address` | Find the nearest symbol for an address. |
| `print &Symbol` | Display a symbol's address. |
| `maintenance info sections` | Show ELF sections and loaded addresses. |

Examples:

```gdb
info functions Timer
info variables Scheduler
info symbol $rip
print/x &kmain
```

## Watchpoints

Watchpoints stop when a value changes or is accessed.

| Command | Explanation |
|---|---|
| `watch Variable` | Stop when the value is written and changes. |
| `rwatch Variable` | Stop when the value is read. |
| `awatch Variable` | Stop when the value is read or written. |
| `watch *(unsigned long long*)Address` | Watch an explicit 64-bit memory address. |
| `info breakpoints` | List breakpoints and watchpoints together. |

Hardware watchpoint availability is limited by the CPU and QEMU target.

## Threads and CPUs

GDB may represent QEMU virtual CPUs as threads.

| Command | Explanation |
|---|---|
| `info threads` | List the CPUs or threads visible to GDB. |
| `thread 2` | Switch to thread or CPU number 2. |
| `thread apply all bt` | Show a backtrace for every visible CPU/thread. |
| `thread apply all info registers` | Show registers for every visible CPU/thread. |

These commands inspect debugger-visible CPUs. They are separate from any
user-level or kernel-level thread objects maintained by the scheduler.

## Catching faults and investigating crashes

When execution stops unexpectedly, start with:

```gdb
info registers
x/16i $rip
x/16xg $rsp
backtrace
info symbol $rip
```

Then inspect the faulting source:

```gdb
list
disassemble /m
```

For an address reported by a kernel panic:

```gdb
info symbol 0xADDRESS
list *0xADDRESS
disassemble 0xSTART, 0xEND
```

## Logging a GDB session

| Command | Explanation |
|---|---|
| `set logging file gdb-session.log` | Select the log file. |
| `set logging overwrite on` | Replace the previous log instead of appending. |
| `set logging enabled on` | Begin logging GDB input and output. |
| `set logging enabled off` | Stop logging. |

## Useful command sequences

### Inspect the current stop

```gdb
info registers
x/10i $rip
backtrace
info locals
```

### Stop at a function and inspect its arguments

```gdb
break FunctionName
continue
info args
info locals
```

### Investigate a memory corruption

```gdb
watch *(unsigned long long*)0xADDRESS
continue
backtrace
x/8xg 0xADDRESS
```

### Debug early assembly

```gdb
break *0xADDRESS
continue
layout asm
info registers
stepi
```

## OESDK debugging files

A Debug build normally creates:

```text
Build\Debug\kernel.elf
Build\Debug\liboesdk.a
Build\Debug\oesdk-gdb-init.cmd
Build\Debug\Start-Gdb-Session.cmd
Build\Debug\serial-debug.log
Build\Debug\oesdk-debug.log
Build\Debug\oesdk-launcher.log
```

`kernel.elf` contains the executable kernel and its debugging symbols.
`serial-debug.log` contains guest serial output, while the OESDK logs record
launcher and debugger startup information.
