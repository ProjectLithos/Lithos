#include "internal.h"
#include <oesdk/console.h>
#include <oesdk/panic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static volatile bool PanicActive;

static inline void disable_interrupts(void) {
    __asm__ volatile ("cli" ::: "memory");
}

static __attribute__((noreturn)) void halt_cpu(void) {
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

static const char *safe_text(const char *Text) {
    return Text != NULL ? Text : "(unspecified)";
}

static void emit_panic_report(
    const char *Module,
    const char *Reason,
    uint64_t ErrorCode,
    const char *File,
    const char *Function,
    uint32_t Line) {
    const char *SafeModule = safe_text(Module);
    const char *SafeReason = safe_text(Reason);

    /* Serial is deliberately written first because it remains useful when the
       video path or framebuffer is the source of the failure. */
    oesdk_serial_printf("\n[FAIL] Kernel panic\n");
    oesdk_serial_printf("Module: %s\n", SafeModule);
    oesdk_serial_printf("Reason: %s\n", SafeReason);
    oesdk_serial_printf("Error: 0x%llX\n", ErrorCode);
    if (File != NULL) oesdk_serial_printf("File: %s:%u\n", File, Line);
    if (Function != NULL) oesdk_serial_printf("Function: %s\n", Function);

    kprintf("\n[FAIL] Kernel panic\n");
    kprintf("Module: %s\n", SafeModule);
    kprintf("Reason: %s\n", SafeReason);
    kprintf("Error: 0x%llX\n", ErrorCode);
    if (File != NULL) kprintf("File: %s:%u\n", File, Line);
    if (Function != NULL) kprintf("Function: %s\n", Function);
}

__attribute__((noreturn)) void OesdkPanicAt(
    const char *Module,
    const char *Reason,
    uint64_t ErrorCode,
    const char *File,
    const char *Function,
    uint32_t Line) {
    disable_interrupts();

    if (PanicActive) {
        oesdk_serial_write("\n[FAIL] Recursive kernel panic; CPU halted.\n");
        halt_cpu();
    }

    PanicActive = true;
    emit_panic_report(Module, Reason, ErrorCode, File, Function, Line);
    halt_cpu();
}

__attribute__((noreturn)) void OesdkPanic(
    const char *Module,
    const char *Reason,
    uint64_t ErrorCode) {
    OesdkPanicAt(Module, Reason, ErrorCode, NULL, NULL, 0U);
}
