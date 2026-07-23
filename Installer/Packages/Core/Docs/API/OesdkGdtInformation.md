# `OesdkGdtInformation`

## Header

```c
#include <oesdk/gdt.h>
```

## Declaration

```c
typedef struct OesdkGdtInformation {
    uint16_t Limit;
    uintptr_t Base;
    uint16_t TaskStateSelector;
    uintptr_t Ring0Stack;
    uintptr_t InterruptStackTable[7];
    uint8_t DescriptorCount;
    bool TaskRegisterLoaded;
    bool Initialized;
} OesdkGdtInformation;
```

## Description

Read-only diagnostic state for the active OESDK-owned GDT and TSS. It reports the GDTR image, TSS selector, current `RSP0`, all seven IST stack tops, descriptor count, task-register state, and initialization state.

Obtain it with `OesdkGdtInformationGet()`.
