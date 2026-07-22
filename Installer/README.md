# OESDK

**OESDK is a freestanding x86-64 operating-system development kit for creating custom kernels and operating systems with C, assembly, Visual Studio, QEMU, and GDB.**

OESDK provides a reusable low-level foundation while leaving the operating-system architecture, policies, services, userland, and interface under the developer's control.

> Current release: **OESDK 0.0.17.1**

## Purpose

OESDK is intended for developers building:

- monolithic kernels;
- microkernels;
- hybrid kernels;
- educational and experimental operating systems;
- specialised embedded or appliance-style systems.

It is an SDK rather than a finished operating system. Projects built with it can select, replace, or extend the supplied components as their design develops.

## Current foundation

The current x86-64 release includes the early kernel foundation needed to boot, enter long mode, initialise the runtime, inspect the boot environment, diagnose fatal errors, query the CPU, install kernel descriptor tables, and handle processor exceptions.

It also includes Visual Studio integration, QEMU launch support, symbol-rich Debug builds, serial diagnostics, and GDB remote debugging.

## Documentation

The detailed interfaces and debugger commands are documented separately:

- [OESDK API Reference](Docs/API/README.md)
- [GDB Command Reference](Docs/GDB-Command-Reference.md)
- [Kernel Debugging Guide](Docs/Kernel-Debugging.md)
- [Subsystem Documentation](Docs/)

Every public API item has its own Markdown page under `Docs/API`. The release checks verify that new public APIs are documented and linked from the API index.

## Minimal kernel

```c
#include <oesdk/kernel.h>

void kmain(int ArgumentCount, char **Arguments)
{
    (void)ArgumentCount;
    (void)Arguments;

    kclear();
    kprintf("Hello from an OESDK kernel.\n");

    for (;;)
    {
        OesdkCpuHalt();
    }
}
```

## Building and debugging

A configured OESDK Visual Studio project can build the kernel, start QEMU, connect GDB, and load the symbols from:

```text
Build/Debug/kernel.elf
```

Press **F5** in a configured project to start the interactive kernel debugging session.

## Repository layout

```text
OESDK/
├── Architecture/       Architecture-specific bootstrap and assembly
├── Docs/               API, subsystem, and debugging documentation
├── Include/oesdk/      Public SDK headers
├── Source/             SDK runtime source
├── Tests/              Release and documentation verification
├── Tools/              Build, launch, and debugging tools
├── VisualStudio/       Visual Studio integration
├── InstallerSource/    Installer source and packaged SDK content
├── Licenses/           Project and third-party licence information
├── manifest.json       Package metadata
├── README.md
└── VERSION
```

## Development direction

The next stages are focused on:

- physical and virtual memory management;
- kernel allocation;
- interrupt controllers and timers;
- threads and scheduling;
- processes and user mode;
- system calls, IPC, and capabilities;
- modular drivers and userland services.

Roadmap items describe intended development and are not claims of completed functionality.

## Versioning

The SDK package and updater are versioned independently.

```text
SDK package version: 0.0.17.1
Updater file version: changes only when updater code changes
```

## Project status

OESDK is under active development. While the SDK remains below `1.0.0`, public interfaces may evolve as the kernel foundation is completed.

## Licence

Project and third-party licence information is provided under [`Licenses/`](Licenses/).
