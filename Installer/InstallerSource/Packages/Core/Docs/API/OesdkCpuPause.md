# `OesdkCpuPause`

**Category:** CPU  
**Kind:** function  
**Header:** `<oesdk/cpu.h>`

Issues the x86 PAUSE hint inside a spin-wait loop.

## Declaration

```c
void OesdkCpuPause(void);
```

## Returns

Nothing.

## Example

```c
while (lock_busy) { OesdkCpuPause(); }
```

[Back to the API index](./README.md)
