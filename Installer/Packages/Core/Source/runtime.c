#include "internal.h"
#include <oesdk/kernel.h>
#include <oesdk/panic.h>
#include <oesdk/cpu.h>
#include <oesdk/gdt.h>
#include <oesdk/interrupt.h>

extern void kmain(int argc, char *argv);

__attribute__((noreturn)) void oesdk_runtime_start(uint32_t multiboot_magic, uintptr_t multiboot_information) {
    oesdk_serial_initialize();
    if (!OesdkCpuInitialize()) {
        OesdkPanic("CPU", "x86-64 long mode is unavailable", 0x0000000000000011ULL);
    }
    if (!OesdkGdtInitialize()) {
        OesdkPanic("GDT", "Initialization failed", 0x0000000000000012ULL);
    }
    if (!OesdkIdtInitialize()) {
        OesdkPanic("IDT", "Initialization failed", 0x0000000000000013ULL);
    }
    if (!OesdkBootContextInitialize(multiboot_magic, multiboot_information)) {
        OesdkPanic("BootContext", "Initialization failed", 0x0000000000000010ULL);
    }
    oesdk_console_initialize();
    oesdk_graphics_initialize(multiboot_magic, multiboot_information);
    kmain(0, NULL);
    kdebugf("OESDK: kmain returned; the CPU will halt.\n");
    for (;;) __asm__ volatile ("cli; hlt");
}
