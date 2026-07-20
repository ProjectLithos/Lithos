/*
 * Project: $safeprojectname$
 * File: kmain.c
 * File Version: 0.0.1
 * Author: __OESDK_AUTHOR_NAME__
 * Email: __OESDK_AUTHOR_EMAIL__
 * License: __OESDK_LICENSE_ID__
 * SPDX-License-Identifier: __OESDK_LICENSE_ID__
 */

#include <oesdk/kernel.h>
#include "../Include/OSVersion.h"

void kmain(int argc, char *argv)
{
    (void)argc;
    (void)argv;

    kdebugf("$safeprojectname$: Hello from the serial debug port!\n");
    kprintf("$safeprojectname$: Hello Screen!\n");
}
