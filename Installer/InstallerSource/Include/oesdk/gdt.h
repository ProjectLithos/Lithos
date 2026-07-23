#ifndef OESDK_GDT_H
#define OESDK_GDT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define OESDK_GDT_NULL_SELECTOR        0x00U
#define OESDK_GDT_KERNEL_CODE_SELECTOR 0x08U
#define OESDK_GDT_KERNEL_DATA_SELECTOR 0x10U
#define OESDK_GDT_USER_DATA_SELECTOR   0x18U
#define OESDK_GDT_USER_CODE_SELECTOR   0x20U
#define OESDK_GDT_TSS_SELECTOR         0x28U

#define OESDK_GDT_USER_DATA_RPL3_SELECTOR (OESDK_GDT_USER_DATA_SELECTOR | 0x03U)
#define OESDK_GDT_USER_CODE_RPL3_SELECTOR (OESDK_GDT_USER_CODE_SELECTOR | 0x03U)

#define OESDK_GDT_IST_DOUBLE_FAULT   1U
#define OESDK_GDT_IST_NMI            2U
#define OESDK_GDT_IST_MACHINE_CHECK  3U
#define OESDK_GDT_IST_SPARE_1        4U
#define OESDK_GDT_IST_SPARE_2        5U
#define OESDK_GDT_IST_SPARE_3        6U
#define OESDK_GDT_IST_SPARE_4        7U
#define OESDK_GDT_IST_ENTRY_COUNT    7U

#define OESDK_GDT_ENTRY_COUNT        7U
#define OESDK_GDT_IST_STACK_SIZE     16384U

typedef enum OesdkGdtDescriptorKind {
    OesdkGdtDescriptorNull = 0,
    OesdkGdtDescriptorKernelCode,
    OesdkGdtDescriptorKernelData,
    OesdkGdtDescriptorUserData,
    OesdkGdtDescriptorUserCode,
    OesdkGdtDescriptorTaskStateSegment
} OesdkGdtDescriptorKind;

typedef struct OesdkGdtInformation {
    uint16_t Limit;
    uintptr_t Base;
    uint16_t TaskStateSelector;
    uintptr_t Ring0Stack;
    uintptr_t InterruptStackTable[OESDK_GDT_IST_ENTRY_COUNT];
    uint8_t DescriptorCount;
    bool TaskRegisterLoaded;
    bool Initialized;
} OesdkGdtInformation;

bool OesdkGdtInitialize(void);
bool OesdkGdtIsInitialized(void);
const OesdkGdtInformation *OesdkGdtInformationGet(void);
bool OesdkGdtSetKernelStack(uintptr_t StackTop);
bool OesdkGdtSetInterruptStack(uint8_t IstIndex, uintptr_t StackTop);

#endif
