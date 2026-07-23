#include "Usage.h"

void TemplateUsageGraphics(void)
{
    const kgfx_surface *Screen;
    if (!kgfx_available()) {
        kprintf("Graphics: no framebuffer surface supplied.\n");
        return;
    }
    Screen = kgfx_screen();
    OESDK_ASSERT(Screen != NULL);
    kgfx_rectangle(8U, 8U, Screen->width > 16U ? Screen->width - 16U : 0U,
                   4U, 0x40C080U);
    kgfx_pixel(8U, 16U, 0xFFFFFFU);
    kprintf("Graphics: %ux%u pitch=%u.\n", Screen->width, Screen->height, Screen->pitch);
}
