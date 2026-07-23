#ifndef OESDK_PROCESS_H
#define OESDK_PROCESS_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/address_space.h>
#include <oesdk/scheduler.h>
#include <oesdk/status.h>
#ifdef __cplusplus
extern "C" {
#endif
#define OESDK_PROCESS_MAX_COUNT 64U
#define OESDK_PROCESS_NAME_MAX 63U
#define OESDK_USER_STACK_DEFAULT_BYTES (64U*1024U)
typedef uint64_t OesdkProcessId;
typedef enum OesdkProcessState { OesdkProcessStateCreated=0,OesdkProcessStateRunning,OesdkProcessStateTerminated } OesdkProcessState;
typedef struct OesdkProcess { OesdkProcessId Id; char Name[OESDK_PROCESS_NAME_MAX+1U]; OesdkProcessState State; OesdkAddressSpace AddressSpace; OesdkThreadId MainThreadId; uintptr_t UserStackBase; size_t UserStackBytes; int32_t ExitCode; } OesdkProcess;
OesdkStatus OesdkProcessCreate(const char *Name,uint64_t RootPhysicalAddress,OesdkProcessId *ProcessId);
OesdkStatus OesdkProcessGet(OesdkProcessId ProcessId,OesdkProcess **Process);
OesdkStatus OesdkProcessConfigureUserStack(OesdkProcessId ProcessId,uintptr_t VirtualBase,uintptr_t PhysicalBase,size_t Bytes);
OesdkStatus OesdkProcessTerminate(OesdkProcessId ProcessId,int32_t ExitCode);
#ifdef __cplusplus
}
#endif
#endif
