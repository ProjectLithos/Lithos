#include "internal.h"
#include <oesdk/kernel.h>
#include <oesdk/panic.h>
#include <oesdk/cpu.h>
#include <oesdk/gdt.h>
#include <oesdk/interrupt.h>
#include <oesdk/interrupt_controller.h>
#include <oesdk/timer.h>

extern void kmain(int argc, char *argv);

__attribute__((noreturn)) void oesdk_runtime_start(uint32_t multiboot_magic, uintptr_t multiboot_information) {
    oesdk_serial_initialize();
    if (!OesdkCpuInitialize()) {
        OesdkPanic("CPU", "x86-64 long mode is unavailable", 0x0000000000000011ULL);
    }
    if (!OesdkGdtInitialize()) {
        OesdkPanic("GDT", "Initialization failed", 0x0000000000000012ULL);
    }
    {
        OesdkStatus Status = OesdkIdtInitialize();
        if (OESDK_FAILED(Status)) {
            OesdkPanic("IDT", "Initialization failed", 0x0000000000000013ULL);
        }
    }
    if (!OesdkBootContextInitialize(multiboot_magic, multiboot_information)) {
        OesdkPanic("BootContext", "Initialization failed", 0x0000000000000010ULL);
    }
    oesdk_console_initialize();
    oesdk_graphics_initialize(multiboot_magic, multiboot_information);
    {
        OesdkStatus Status = OesdkInterruptControllerInitializePic(OESDK_PIC_VECTOR_BASE);
        if (OESDK_FAILED(Status)) {
            OesdkPanic("InterruptController", "PIC initialization failed", 0x0000000000000014ULL);
        }
        Status = OesdkTimerInitializePit(100U, OESDK_TIMER_DEFAULT_VECTOR);
        if (OESDK_FAILED(Status)) {
            OesdkPanic("Timer", "PIT initialization failed", 0x0000000000000015ULL);
        }
        OesdkCpuEnableInterrupts();
    }
    kmain(0, NULL);
    kdebugf("OESDK: kmain returned; the CPU will halt.\n");
    for (;;) __asm__ volatile ("cli; hlt");
}
