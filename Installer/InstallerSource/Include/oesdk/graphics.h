#ifndef OESDK_GRAPHICS_H
#define OESDK_GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct kgfx_surface {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bits_per_pixel;
    volatile uint8_t *pixels;
} kgfx_surface;

bool kgfx_available(void);
const kgfx_surface *kgfx_screen(void);
void kgfx_pixel(uint32_t x, uint32_t y, uint32_t rgb);
void kgfx_fill(uint32_t rgb);
void kgfx_rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t rgb);

#endif
