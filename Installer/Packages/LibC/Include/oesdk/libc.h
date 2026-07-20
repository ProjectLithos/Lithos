#ifndef OESDK_LIBC_H
#define OESDK_LIBC_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_LIBC_ABI_VERSION 1u

typedef struct OesdkLibcHost {
    uint32_t AbiVersion;
    void *Context;

    ptrdiff_t (*Read)(void *Context, int FileDescriptor, void *Buffer, size_t Count);
    ptrdiff_t (*Write)(void *Context, int FileDescriptor, const void *Buffer, size_t Count);
    int (*Open)(void *Context, const char *Path, int Flags, int Mode);
    int (*Close)(void *Context, int FileDescriptor);
    int64_t (*Seek)(void *Context, int FileDescriptor, int64_t Offset, int Whence);
    int (*Stat)(void *Context, const char *Path, void *StatBuffer);
    int (*FileStat)(void *Context, int FileDescriptor, void *StatBuffer);
    int (*IsTerminal)(void *Context, int FileDescriptor);
    int (*Unlink)(void *Context, const char *Path);
    int (*Rename)(void *Context, const char *OldPath, const char *NewPath);

    void *(*HeapGrow)(void *Context, ptrdiff_t Increment);
    int (*ClockGetTime)(void *Context, int ClockId, int64_t *Seconds, int32_t *Nanoseconds);
    int (*GetProcessId)(void *Context);
    int (*Kill)(void *Context, int ProcessId, int Signal);
    void (*ExitProcess)(void *Context, int Status);

    void (*Lock)(void *Context, unsigned LockId);
    void (*Unlock)(void *Context, unsigned LockId);
} OesdkLibcHost;

int OesdkLibcInstallHost(const OesdkLibcHost *Host);
const OesdkLibcHost *OesdkLibcGetHost(void);
void OesdkLibcInitialize(void);

#ifdef __cplusplus
}
#endif
#endif
