#include "Usage.h"

void kmain(int argc, char *argv)
{
    const OesdkBootContext *BootContext;
    uint64_t SavedInterruptState;

    (void)argc;
    (void)argv;

    kclear();
    kputs("$safeprojectname$: OESDK template kernel\n");
    kdebugf("$safeprojectname$: SDK usage modules are starting.\n");

    BootContext = OesdkBootContextGet();
    OESDK_ASSERT_CODE(BootContext != NULL, 0x18200001ULL);

    TemplateUsageBoot(BootContext);
    TemplateUsageCpu();
    TemplateUsageMemory(BootContext);
    TemplateUsageInterruptsAndTimer();
    TemplateUsageObjects();
    TemplateUsageGraphics();

    SavedInterruptState = OesdkCpuInterruptStateSave();
    OesdkCpuPause();
    OesdkCpuInterruptStateRestore(SavedInterruptState);

    kprintf("$safeprojectname$: all safe template SDK usage completed.\n");
}
