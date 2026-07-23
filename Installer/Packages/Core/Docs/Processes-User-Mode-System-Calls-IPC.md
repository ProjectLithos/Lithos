# Processes, User Mode, System Calls, IPC and Capabilities

OESDK 0.17.36 introduces reusable process and user-mode foundations without forcing a monolithic, microkernel, or hybrid architecture.

## Security invariants

Userland receives handles, IDs and capabilities, never arbitrary kernel pointers. `OesdkUserRangeValidate` rejects overflow, absent pages, supervisor-only pages, writes to read-only pages, and ranges crossing the lower canonical user boundary.

The existing x86-64 GDT owns Ring 3 code/data descriptors. `OesdkUserModeFramePrepare` creates a controlled IRET frame and `OesdkUserModeEnter` performs the final transition after the kernel has selected the process CR3 and TSS ring-zero stack.

## ELF64

`OesdkElfLoadExecutable64` validates ELF64 little-endian x86-64 executable headers, bounds-checks program headers and segments, allocates pages through caller services, maps each loadable segment into the process address space, and applies writable/NX permissions.

## System calls

The generic ABI remains exactly `Get`, `Set`, and `Event`. The x86-64 register contract is RAX, RDI, RSI, RDX, R10, R8 and R9. Non-negative RAX values are results; negative values are `OesdkStatus` errors. Dispatcher buffer checks use the caller's address space before invoking a handler.

## IPC and capabilities

Channels and endpoints provide bounded messages. Capability transfer requires transfer rights. Derived capabilities always use `ParentRights & RequestedRights`; rights can never be invented.

## Architecture placement

Portable modules compile independently. The manifest chooses `Kernel`, `User`, `Excluded`, or `Automatic`. Monolithic builds link selected services into `kernel.elf`; microkernels retain only core memory, scheduling, IPC and capabilities in kernel mode; hybrid builds choose performance-critical services individually.
