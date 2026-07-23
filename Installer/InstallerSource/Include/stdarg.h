#ifndef OESDK_STDARG_H
#define OESDK_STDARG_H

typedef __builtin_va_list va_list;
#define va_start(list, last) __builtin_va_start(list, last)
#define va_arg(list, type) __builtin_va_arg(list, type)
#define va_end(list) __builtin_va_end(list)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

#endif
