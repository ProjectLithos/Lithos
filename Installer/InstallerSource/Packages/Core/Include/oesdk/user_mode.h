#ifndef OESDK_USER_MODE_H
#define OESDK_USER_MODE_H
#include <stdint.h>
#include <oesdk/gdt.h>
#include <oesdk/status.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct OesdkUserModeFrame { uintptr_t InstructionPointer; uintptr_t StackPointer; uint64_t Flags; uint16_t CodeSelector; uint16_t DataSelector; } OesdkUserModeFrame;
OesdkStatus OesdkUserModeFramePrepare(uintptr_t EntryPoint,uintptr_t StackTop,OesdkUserModeFrame *Frame);
#if defined(__x86_64__)
__attribute__((noreturn)) void OesdkUserModeEnter(const OesdkUserModeFrame *Frame);
#endif
#ifdef __cplusplus
}
#endif
#endif
