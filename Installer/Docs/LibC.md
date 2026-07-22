# OESDK 0.0.16 full libc

OESDK 0.0.16 integrates Newlib 4.6.0.20260123 as its full C library.

Newlib supplies the standard C library and mathematical library. OESDK supplies
the operating-system boundary through `OesdkLibcHost`. Install the host table
before the first call to allocation, stdio, files, clocks, process functions,
or a libc operation which may acquire a lock.

## Build

On Windows with WSL:

```powershell
.\Tools\BuildNewlib.ps1
```

On Linux/WSL:

```sh
./Tools/BuildNewlib.sh
```

The build requires `x86_64-elf-gcc`, `x86_64-elf-ar`, make, curl, tar and the
usual Newlib build prerequisites. Set `OESDK_TARGET` for another ELF target.

## Linking

Use the matching Debug or Release include and library directories. Link in this
order:

```text
application objects
-loesdk-newlib-port
-lc
-lm
```

When using a static linker group:

```text
-Wl,--start-group -loesdk-newlib-port -lc -lm -Wl,--end-group
```

## Required host operations

`Write`, `HeapGrow`, and `ExitProcess` are mandatory. Other absent operations
return a suitable error such as `ENOSYS`. This lets console-only kernels use
`printf`, allocation, strings, conversions, sorting, locale-independent
functions, and mathematics before a filesystem or process model exists.

For concurrent userland, provide `Lock` and `Unlock`. The Newlib build enables
reentrancy and retargetable locking.
