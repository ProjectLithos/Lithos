# `OesdkGdtDescriptorKind`

## Header

```c
#include <oesdk/gdt.h>
```

## Declaration

```c
typedef enum OesdkGdtDescriptorKind { ... } OesdkGdtDescriptorKind;
```

## Description

Names the logical descriptor roles in the kernel-owned GDT: null, kernel code, kernel data, user data, user code, and TSS.
