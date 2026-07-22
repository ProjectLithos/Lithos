#include "internal.h"
#include <oesdk/debug.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#define COM1 0x3F8U

static bool Initialized;

static inline void out8(uint16_t Port, uint8_t Value) {
    __asm__ volatile ("outb %0, %1" : : "a"(Value), "Nd"(Port));
}

static inline uint8_t in8(uint16_t Port) {
    uint8_t Value;
    __asm__ volatile ("inb %1, %0" : "=a"(Value) : "Nd"(Port));
    return Value;
}

void oesdk_serial_initialize(void) {
    out8(COM1 + 1U, 0x00U);
    out8(COM1 + 3U, 0x80U);
    out8(COM1 + 0U, 0x03U);
    out8(COM1 + 1U, 0x00U);
    out8(COM1 + 3U, 0x03U);
    out8(COM1 + 2U, 0xC7U);
    out8(COM1 + 4U, 0x0BU);
    Initialized = true;
}

static void serial_emit(char Character, void *Context) {
    (void)Context;
    if (!Initialized) oesdk_serial_initialize();
    if (Character == '\n') serial_emit('\r', NULL);
    while ((in8(COM1 + 5U) & 0x20U) == 0U) { }
    out8(COM1, (uint8_t)Character);
}

void oesdk_serial_write(const char *Text) {
    if (Text == NULL) return;
    while (*Text != '\0') serial_emit(*Text++, NULL);
}

int oesdk_serial_printf(const char *Format, ...) {
    va_list Arguments;
    va_start(Arguments, Format);
    int Result = oesdk_vformat(serial_emit, NULL, Format, Arguments);
    va_end(Arguments);
    return Result;
}

#ifdef DEBUG
int oesdk_debug_printf(const char *Format, ...) {
    va_list Arguments;
    va_start(Arguments, Format);
    int Result = oesdk_vformat(serial_emit, NULL, Format, Arguments);
    va_end(Arguments);
    return Result;
}
#endif
