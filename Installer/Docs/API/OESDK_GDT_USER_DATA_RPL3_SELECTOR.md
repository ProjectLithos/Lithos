# `OESDK_GDT_USER_DATA_RPL3_SELECTOR`

## Header

```c
#include <oesdk/gdt.h>
```

## Declaration

```c
#define OESDK_GDT_USER_DATA_RPL3_SELECTOR (OESDK_GDT_USER_DATA_SELECTOR | 0x03U)
```

## Description

Produces the ring-3 user-data selector value `0x1B` from the user-data GDT slot at `0x18`.
