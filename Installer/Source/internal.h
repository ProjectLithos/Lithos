#ifndef OESDK_INTERNAL_H
#define OESDK_INTERNAL_H

#include <stdarg.h>
#include <stdint.h>

typedef void (*oesdk_emit_character)(char character, void *context);

int oesdk_vformat(oesdk_emit_character emit, void *context, const char *format, va_list arguments);
void oesdk_console_initialize(void);
void oesdk_serial_initialize(void);
void oesdk_serial_write(const char *Text);
int oesdk_serial_printf(const char *Format, ...);
void oesdk_graphics_initialize(uint32_t multiboot_magic, uintptr_t multiboot_information);

#endif
