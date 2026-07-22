# `OESDK_GDT_USER_CODE_RPL3_SELECTOR`

## Header

```c
#include <oesdk/gdt.h>
```

## Declaration

```c
#define OESDK_GDT_USER_CODE_RPL3_SELECTOR (OESDK_GDT_USER_CODE_SELECTOR | 0x03U)
```

## Description

Produces the ring-3 user-code selector value `0x23` from the user-code GDT slot at `0x20`.
