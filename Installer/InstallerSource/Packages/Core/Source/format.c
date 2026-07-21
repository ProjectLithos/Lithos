#include "internal.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct format_output {
    oesdk_emit_character emit;
    void *context;
    int count;
} format_output;

static void output_character(format_output *output, char character) {
    output->emit(character, output->context);
    ++output->count;
}

static void output_unsigned(format_output *output, uint64_t value, unsigned base, bool uppercase) {
    char buffer[32];
    size_t length = 0;
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    do {
        buffer[length++] = digits[value % base];
        value /= base;
    } while (value != 0);
    while (length != 0) output_character(output, buffer[--length]);
}

int oesdk_vformat(oesdk_emit_character emit, void *context, const char *format, va_list arguments) {
    format_output output = { emit, context, 0 };
    while (*format != '\0') {
        if (*format != '%') {
            output_character(&output, *format++);
            continue;
        }
        ++format;
        if (*format == '%') {
            output_character(&output, *format++);
            continue;
        }
        int length = 0;
        if (*format == 'l') {
            ++length;
            ++format;
            if (*format == 'l') { ++length; ++format; }
        } else if (*format == 'z') {
            length = 2;
            ++format;
        }

        char conversion = *format == '\0' ? '\0' : *format++;
        if (conversion == 'c') {
            output_character(&output, (char)va_arg(arguments, int));
        } else if (conversion == 's') {
            const char *text = va_arg(arguments, const char *);
            if (text == NULL) text = "(null)";
            while (*text != '\0') output_character(&output, *text++);
        } else if (conversion == 'd' || conversion == 'i') {
            int64_t value = length ? va_arg(arguments, int64_t) : va_arg(arguments, int);
            uint64_t magnitude = (uint64_t)value;
            if (value < 0) {
                output_character(&output, '-');
                magnitude = 0 - magnitude;
            }
            output_unsigned(&output, magnitude, 10, false);
        } else if (conversion == 'u' || conversion == 'x' || conversion == 'X') {
            uint64_t value = length ? va_arg(arguments, uint64_t) : va_arg(arguments, unsigned int);
            output_unsigned(&output, value, conversion == 'u' ? 10U : 16U, conversion == 'X');
        } else if (conversion == 'p') {
            uintptr_t value = (uintptr_t)va_arg(arguments, void *);
            output_character(&output, '0');
            output_character(&output, 'x');
            output_unsigned(&output, value, 16, false);
        } else {
            output_character(&output, '%');
            if (conversion != '\0') output_character(&output, conversion);
        }
    }
    return output.count;
}
