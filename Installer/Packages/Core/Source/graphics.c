#include "internal.h"
#include <oesdk/graphics.h>
#include <stddef.h>

static kgfx_surface screen;
static bool ready;

static inline void out8(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

#ifdef OESDK_DESKTOP
static inline uint8_t in8(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void write_vga_registers(void) {
    static uint8_t registers[] = {
        0x63,
        0x03, 0x01, 0x0F, 0x00, 0x0E,
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x41, 0x00, 0x0F, 0x00, 0x00
    };
    size_t index = 0;
    out8(0x3C2, registers[index++]);
    for (uint8_t register_index = 0; register_index < 5; ++register_index) {
        out8(0x3C4, register_index);
        out8(0x3C5, registers[index++]);
    }
    out8(0x3D4, 0x03);
    out8(0x3D5, in8(0x3D5) | 0x80);
    out8(0x3D4, 0x11);
    out8(0x3D5, in8(0x3D5) & 0x7F);
    registers[index + 0x03] |= 0x80;
    registers[index + 0x11] &= 0x7F;
    for (uint8_t register_index = 0; register_index < 25; ++register_index) {
        out8(0x3D4, register_index);
        out8(0x3D5, registers[index++]);
    }
    for (uint8_t register_index = 0; register_index < 9; ++register_index) {
        out8(0x3CE, register_index);
        out8(0x3CF, registers[index++]);
    }
    for (uint8_t register_index = 0; register_index < 21; ++register_index) {
        (void)in8(0x3DA);
        out8(0x3C0, register_index);
        out8(0x3C0, registers[index++]);
    }
    (void)in8(0x3DA);
    out8(0x3C0, 0x20);

    out8(0x3C8, 0);
    for (unsigned colour = 0; colour < 256; ++colour) {
        out8(0x3C9, (uint8_t)((((colour >> 5) & 7U) * 63U) / 7U));
        out8(0x3C9, (uint8_t)((((colour >> 2) & 7U) * 63U) / 7U));
        out8(0x3C9, (uint8_t)(((colour & 3U) * 63U) / 3U));
    }
}
#endif

void oesdk_graphics_initialize(uint32_t magic, uintptr_t information) {
    (void)magic;
    (void)information;
    ready = false;
#ifdef OESDK_DESKTOP
    write_vga_registers();
    screen.width = 320;
    screen.height = 200;
    screen.pitch = 320;
    screen.bits_per_pixel = 8;
    screen.pixels = (volatile uint8_t *)(uintptr_t)0xA0000;
    ready = true;
#endif
}

bool kgfx_available(void) { return ready; }
const kgfx_surface *kgfx_screen(void) { return ready ? &screen : NULL; }

static uint8_t rgb_to_palette(uint32_t rgb) {
    uint8_t red = (uint8_t)(rgb >> 16);
    uint8_t green = (uint8_t)(rgb >> 8);
    uint8_t blue = (uint8_t)rgb;
    return (uint8_t)((red & 0xE0U) | ((green & 0xE0U) >> 3) | (blue >> 6));
}

void kgfx_pixel(uint32_t x, uint32_t y, uint32_t rgb) {
    if (!ready || x >= screen.width || y >= screen.height) return;
    screen.pixels[y * screen.pitch + x] = rgb_to_palette(rgb);
}

void kgfx_rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t rgb) {
    if (!ready || x >= screen.width || y >= screen.height) return;
    uint32_t right = width > screen.width - x ? screen.width : x + width;
    uint32_t bottom = height > screen.height - y ? screen.height : y + height;
    uint8_t colour = rgb_to_palette(rgb);
    for (uint32_t row = y; row < bottom; ++row) {
        for (uint32_t column = x; column < right; ++column) screen.pixels[row * screen.pitch + column] = colour;
    }
}

void kgfx_fill(uint32_t rgb) {
    if (ready) kgfx_rectangle(0, 0, screen.width, screen.height, rgb);
}
