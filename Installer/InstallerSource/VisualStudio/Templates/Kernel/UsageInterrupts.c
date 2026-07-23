#include "Usage.h"

static void TemplateInterruptHandler(OesdkInterruptFrame *Frame, void *Context)
{
    (void)Context;
    if (Frame != NULL) kdebugf("Template interrupt vector %u received.\n", (unsigned int)Frame->Vector);
}

void TemplateUsageInterruptsAndTimer(void)
{
    OesdkStatus Status = OesdkInterruptHandlerRegister(
        OESDK_INTERRUPT_VECTOR_FIRST, TemplateInterruptHandler, NULL);
    const OesdkInterruptControllerInformation *Controller = OesdkInterruptControllerInformationGet();
    const OesdkTimerInformation *Timer = OesdkTimerInformationGet();

    kprintf("Interrupt registration: %s\n", OesdkStatusName(Status));
    if (OESDK_SUCCEEDED(Status)) {
        OESDK_ASSERT(OesdkInterruptHandlerIsRegistered(OESDK_INTERRUPT_VECTOR_FIRST));
        OESDK_ASSERT(OESDK_SUCCEEDED(OesdkInterruptHandlerUnregister(
            OESDK_INTERRUPT_VECTOR_FIRST, TemplateInterruptHandler)));
    }

    if (Controller != NULL && Controller->Initialized) {
        kprintf("Interrupt controller type=%u vector-base=%u\n",
                (unsigned int)Controller->Type, (unsigned int)Controller->VectorBase);
    }
    if (Timer != NULL && Timer->Initialized) {
        kprintf("Timer source=%u actual=%u ticks=%llu; 250 ticks at 100 Hz=%llu ms\n",
                (unsigned int)Timer->Source, (unsigned int)Timer->ActualHz,
                (unsigned long long)OesdkTimerTicks(),
                (unsigned long long)OesdkTimerTicksToMilliseconds(250U, 100U));
        (void)OesdkTimerTicksToNanoseconds(250U, 100U);
    }
}
