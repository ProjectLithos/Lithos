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
    kdebugf("$safeprojectname$: starting graphical desktop.\n");

    if (!kgfx_available()) {
        kprintf("A linear framebuffer was not supplied by the boot loader.\n");
        return;
    }

    const kgfx_surface *screen = kgfx_screen();
    kgfx_fill(0x183153);
    kgfx_rectangle(0, screen->height - 48, screen->width, 48, 0x101820);
    kgfx_rectangle(48, 48, screen->width > 96 ? screen->width - 96 : 0, screen->height > 144 ? screen->height - 144 : 0, 0xE8EDF2);
    kgfx_rectangle(48, 48, screen->width > 96 ? screen->width - 96 : 0, 36, 0x2878B8);
    kgfx_rectangle(72, 108, 240, 160, 0xFFFFFF);
    kgfx_rectangle(72, 108, 240, 28, 0x36A3D9);
}
