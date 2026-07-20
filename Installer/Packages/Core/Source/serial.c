#include "internal.h"
#include <oesdk/debug.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

enum { COM1 = 0x3F8 };
static bool initialized;

static inline void out8(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t in8(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void oesdk_serial_initialize(void) {
    out8(COM1 + 1, 0x00);
    out8(COM1 + 3, 0x80);
    out8(COM1 + 0, 0x03);
    out8(COM1 + 1, 0x00);
    out8(COM1 + 3, 0x03);
    out8(COM1 + 2, 0xC7);
    out8(COM1 + 4, 0x0B);
    initialized = true;
}

#ifdef DEBUG
static void serial_emit(char character, void *context) {
    (void)context;
    if (!initialized) oesdk_serial_initialize();
    if (character == '\n') serial_emit('\r', NULL);
    while ((in8(COM1 + 5) & 0x20U) == 0) { }
    out8(COM1, (uint8_t)character);
}

int oesdk_debug_printf(const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    int result = oesdk_vformat(serial_emit, NULL, format, arguments);
    va_end(arguments);
    return result;
}
#endif
