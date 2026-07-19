#include <oesdk/kernel.h>

void kmain(int argc, char *argv)
{
    (void)argc;
    (void)argv;

    kdebugf("$safeprojectname$: Hello from the serial debug port!\n");
    kprintf("$safeprojectname$: Hello Screen!\n");
}
