#ifndef OESDK_INTERRUPT_CONTROLLER_H
#define OESDK_INTERRUPT_CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_IRQ_COUNT 16U
#define OESDK_PIC_VECTOR_BASE 32U
#define OESDK_PIC_VECTOR_LAST 47U
#define OESDK_LOCAL_APIC_DEFAULT_BASE UINT64_C(0xFEE00000)

#define OESDK_IOAPIC_DELIVERY_FIXED       UINT64_C(0)
#define OESDK_IOAPIC_DESTINATION_PHYSICAL UINT64_C(0)
#define OESDK_IOAPIC_POLARITY_LOW         (UINT64_C(1) << 13U)
#define OESDK_IOAPIC_TRIGGER_LEVEL        (UINT64_C(1) << 15U)
#define OESDK_IOAPIC_MASKED               (UINT64_C(1) << 16U)

typedef enum OesdkInterruptControllerType
{
    OesdkInterruptControllerNone = 0,
    OesdkInterruptControllerPic,
    OesdkInterruptControllerLocalApic,
    OesdkInterruptControllerX2Apic
} OesdkInterruptControllerType;

typedef struct OesdkInterruptControllerInformation
{
    OesdkInterruptControllerType Type;
    uintptr_t LocalApicBase;
    uintptr_t IoApicBase;
    uint8_t VectorBase;
    uint8_t LocalApicId;
    bool Initialized;
    bool IoApicAvailable;
} OesdkInterruptControllerInformation;

OesdkStatus OesdkInterruptControllerInitializePic(uint8_t VectorBase);
OesdkStatus OesdkInterruptControllerInitializeLocalApic(uintptr_t LocalApicBase);
OesdkStatus OesdkInterruptControllerInitializeX2Apic(void);
OesdkStatus OesdkInterruptControllerAttachIoApic(uintptr_t IoApicBase);
OesdkStatus OesdkInterruptControllerRouteIrq(uint8_t Irq, uint8_t Vector, uint32_t DestinationApicId, uint64_t Flags);
OesdkStatus OesdkInterruptControllerMaskIrq(uint8_t Irq);
OesdkStatus OesdkInterruptControllerUnmaskIrq(uint8_t Irq);
void OesdkInterruptControllerEndOfInterrupt(uint8_t Vector);
bool OesdkInterruptControllerOwnsVector(uint8_t Vector);
const OesdkInterruptControllerInformation *OesdkInterruptControllerInformationGet(void);

#ifdef __cplusplus
}
#endif

#endif
