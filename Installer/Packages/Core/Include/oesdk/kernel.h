#ifndef OESDK_KERNEL_H
#define OESDK_KERNEL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <oesdk/console.h>
#include <oesdk/debug.h>
#include <oesdk/graphics.h>
#include <oesdk/boot_context.h>
#include <oesdk/panic.h>
#include <oesdk/cpu.h>
#include <oesdk/status.h>
#include <oesdk/gdt.h>
#include <oesdk/interrupt.h>
#include <oesdk/memory_map.h>
#include <oesdk/physical_memory.h>
#include <oesdk/virtual_memory.h>
#include <oesdk/heap.h>
#include <oesdk/timer.h>
#include <oesdk/interrupt_controller.h>

#define OESDK_VERSION_MAJOR 0
#define OESDK_VERSION_MINOR 17
#define OESDK_VERSION_PATCH 33
#define OESDK_VERSION_REVISION 0

#endif
