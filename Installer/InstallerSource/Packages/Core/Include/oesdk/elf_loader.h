#ifndef OESDK_ELF_LOADER_H
#define OESDK_ELF_LOADER_H
#include <stddef.h>
#include <stdint.h>
#include <oesdk/process.h>
#include <oesdk/status.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef OesdkStatus (*OesdkElfAllocatePages)(size_t PageCount,uintptr_t *PhysicalAddress,void *Context);
typedef OesdkStatus (*OesdkElfCopyToPhysical)(uintptr_t PhysicalAddress,const void *Data,size_t Size,size_t Offset,void *Context);
typedef struct OesdkElfLoadServices { OesdkElfAllocatePages AllocatePages; OesdkElfCopyToPhysical CopyToPhysical; void *Context; } OesdkElfLoadServices;
typedef struct OesdkElfImageInformation { uintptr_t EntryPoint; size_t SegmentCount; uintptr_t LowestAddress; uintptr_t HighestAddress; } OesdkElfImageInformation;
OesdkStatus OesdkElfLoadExecutable64(OesdkProcess *Process,const void *Image,size_t ImageSize,const OesdkElfLoadServices *Services,OesdkElfImageInformation *Information);
#ifdef __cplusplus
}
#endif
#endif
