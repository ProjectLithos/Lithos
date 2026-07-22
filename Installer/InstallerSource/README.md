# OESDK 0.0.17.1

OESDK is a freestanding x86-64 operating-system development kit with Visual Studio, QEMU, GDB, bootstrap, runtime, console, graphics, Boot Context, panic, CPU, status, GDT/TSS, and IDT/exception support.

## API documentation

- [Complete API reference](Documentation/API/README.md)
- [Boot Context overview](Documentation/Boot-Context.md)
- [Panic and assertions overview](Documentation/Panic-and-Assertions.md)
- [CPU primitives overview](Documentation/CPU-Primitives.md)
- [Status and errors overview](Documentation/Status-and-Errors.md)
- [GDT and TSS overview](Documentation/GDT-and-TSS.md)
- [IDT and exceptions overview](Documentation/IDT-and-Exceptions.md)
- [GDB command reference](Documentation/GDB-Command-Reference.md)

Every public API function, function-like macro, and public SDK type has its own Markdown page under `Documentation/API`. `Tests/VerifyApiDocumentation.py` checks that new public APIs cannot be packaged without documentation and an index link.

## Public umbrella header

```c
#include <oesdk/kernel.h>
```

Individual subsystem headers can also be included directly.

## Build and debugging

OESDK projects produce a symbol-rich `Build/Debug/kernel.elf`. Press F5 in a configured Visual Studio project to start QEMU and the interactive GDB session.

## Version

- SDK package: `0.0.17.1`
- API documentation contract introduced in this refreshed package
- The updater keeps its own independent file version and is not versioned from the SDK package number
