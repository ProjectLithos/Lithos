#ifndef OESDK_SYSCALL_H
#define OESDK_SYSCALL_H
#include <stddef.h>
#include <stdint.h>
#include <oesdk/address_space.h>
#include <oesdk/status.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum OesdkSystemCall { OesdkSystemCallGet=0,OesdkSystemCallSet=1,OesdkSystemCallEvent=2 } OesdkSystemCall;
typedef struct OesdkSystemCallArguments { uint64_t Number,Object,Operation; uintptr_t Buffer; size_t BufferLength; uint64_t Flags,OptionalArgument; } OesdkSystemCallArguments;
typedef int64_t (*OesdkSystemCallHandler)(const OesdkSystemCallArguments *Arguments,void *Context);
typedef struct OesdkSystemCallTable { OesdkSystemCallHandler Get,Set,Event; void *Context; } OesdkSystemCallTable;
int64_t OesdkSystemCallDispatch(const OesdkSystemCallTable *Table,const OesdkAddressSpace *CallerAddressSpace,const OesdkSystemCallArguments *Arguments);
#ifdef __cplusplus
}
#endif
#endif
