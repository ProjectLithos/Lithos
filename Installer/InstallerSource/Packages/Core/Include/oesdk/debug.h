#ifndef OESDK_DEBUG_H
#define OESDK_DEBUG_H

#ifdef DEBUG
int oesdk_debug_printf(const char *format, ...);
#define kdebugf(...) oesdk_debug_printf(__VA_ARGS__)
#else
#define kdebugf(...) ((void)0)
#endif

#endif
