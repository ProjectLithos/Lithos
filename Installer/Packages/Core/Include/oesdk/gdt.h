#ifndef OESDK_GDT_H
#define OESDK_GDT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define OESDK_GDT_KERNEL_CODE_SELECTOR 0x08U
#define OESDK_GDT_KERNEL_DATA_SELECTOR 0x10U
#define OESDK_GDT_USER_DATA_SELECTOR   0x1BU
#define OESDK_GDT_USER_CODE_SELECTOR   0x23U
#define OESDK_GDT_TSS_SELECTOR         0x28U

#define OESDK_GDT_IST_DOUBLE_FAULT 1U
#define OESDK_GDT_IST_NMI          2U
#define OESDK_GDT_IST_MACHINE_CHECK 3U

typedef struct OesdkGdtInformation {
    uint16_t Limit;
    uintptr_t Base;
    uint16_t TaskStateSelector;
    uintptr_t Ring0Stack;
    uintptr_t InterruptStackTable[7];
} OesdkGdtInformation;

bool OesdkGdtInitialize(void);
const OesdkGdtInformation *OesdkGdtInformationGet(void);
bool OesdkGdtSetKernelStack(uintptr_t StackTop);

#endif
