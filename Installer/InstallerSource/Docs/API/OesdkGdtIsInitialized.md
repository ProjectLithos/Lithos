# `OesdkGdtIsInitialized`

## Header

```c
#include <oesdk/gdt.h>
```

## Declaration

```c
bool OesdkGdtIsInitialized(void);
```

## Description

Returns `true` after the OESDK-owned GDT has been loaded and the task register has been loaded with the OESDK TSS selector.

## Example

```c
if (!OesdkGdtIsInitialized()) {
    OESDK_ASSERT(OesdkGdtInitialize());
}
```
