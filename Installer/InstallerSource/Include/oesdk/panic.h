#ifndef OESDK_PANIC_H
#define OESDK_PANIC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
#define OESDK_NORETURN __attribute__((noreturn))
#else
#define OESDK_NORETURN
#endif

OESDK_NORETURN void OesdkPanic(
    const char *Module,
    const char *Reason,
    uint64_t ErrorCode);

OESDK_NORETURN void OesdkPanicAt(
    const char *Module,
    const char *Reason,
    uint64_t ErrorCode,
    const char *File,
    const char *Function,
    uint32_t Line);

#define OESDK_PANIC(Module, Reason, ErrorCode) \
    OesdkPanicAt((Module), (Reason), (uint64_t)(ErrorCode), __FILE__, __func__, (uint32_t)__LINE__)

#define OESDK_ASSERT(Expression) \
    do { \
        if (!(Expression)) { \
            OesdkPanicAt("Assertion", #Expression, 0U, __FILE__, __func__, (uint32_t)__LINE__); \
        } \
    } while (0)

#define OESDK_ASSERT_CODE(Expression, ErrorCode) \
    do { \
        if (!(Expression)) { \
            OesdkPanicAt("Assertion", #Expression, (uint64_t)(ErrorCode), __FILE__, __func__, (uint32_t)__LINE__); \
        } \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
