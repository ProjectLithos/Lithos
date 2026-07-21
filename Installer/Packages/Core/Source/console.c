#include "internal.h"
#include <oesdk/console.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

enum { VGA_WIDTH = 80, VGA_HEIGHT = 25 };
static volatile uint16_t *const vga = (volatile uint16_t *)(uintptr_t)0xB8000;
static size_t row;
static size_t column;
static uint8_t colour = 0x0F;

static void scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; ++y) {
        for (size_t x = 0; x < VGA_WIDTH; ++x) vga[(y - 1) * VGA_WIDTH + x] = vga[y * VGA_WIDTH + x];
    }
    for (size_t x = 0; x < VGA_WIDTH; ++x) vga[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ((uint16_t)colour << 8) | ' ';
    row = VGA_HEIGHT - 1;
}

void oesdk_console_initialize(void) {
    row = 0;
    column = 0;
    kclear();
}

void kclear(void) {
    for (size_t index = 0; index < VGA_WIDTH * VGA_HEIGHT; ++index) vga[index] = ((uint16_t)colour << 8) | ' ';
    row = 0;
    column = 0;
}

void kputc(char character) {
    if (character == '\n') {
        column = 0;
        if (++row == VGA_HEIGHT) scroll();
        return;
    }
    if (character == '\r') { column = 0; return; }
    if (character == '\t') { do { kputc(' '); } while ((column & 3U) != 0); return; }
    vga[row * VGA_WIDTH + column] = ((uint16_t)colour << 8) | (uint8_t)character;
    if (++column == VGA_WIDTH) {
        column = 0;
        if (++row == VGA_HEIGHT) scroll();
    }
}

void kputs(const char *text) {
    while (*text != '\0') kputc(*text++);
}

static void console_emit(char character, void *context) {
    (void)context;
    kputc(character);
}

int kprintf(const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    int result = oesdk_vformat(console_emit, NULL, format, arguments);
    va_end(arguments);
    return result;
}
