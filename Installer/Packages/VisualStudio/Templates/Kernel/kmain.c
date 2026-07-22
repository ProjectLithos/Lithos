#include <oesdk/kernel.h>

void kmain(int argc, char *argv)
{
    (void)argc;
    (void)argv;

    const OesdkBootContext *bootContext = OesdkBootContextGet();
    if (bootContext != NULL) {
        kprintf("ABI Version: %u\n", (unsigned int)bootContext->AbiVersion);
    }

    kdebugf("$safeprojectname$: Hello from the serial debug port!\n");
    kprintf("$safeprojectname$: Hello Screen!\n");
}
