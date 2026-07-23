#ifndef OESDK_ADDRESS_SPACE_H
#define OESDK_ADDRESS_SPACE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/status.h>
#include <oesdk/virtual_memory.h>
#ifdef __cplusplus
extern "C" {
#endif
#define OESDK_ADDRESS_SPACE_MAX_MAPPINGS 256U
#define OESDK_USER_ADDRESS_MAX UINT64_C(0x00007FFFFFFFFFFF)
typedef uint64_t OesdkAddressSpaceId;
typedef struct OesdkAddressSpaceMapping { uintptr_t VirtualAddress; uintptr_t PhysicalAddress; size_t PageCount; OesdkVirtualFlags Flags; } OesdkAddressSpaceMapping;
typedef struct OesdkAddressSpace { OesdkAddressSpaceId Id; uint64_t RootPhysicalAddress; size_t MappingCount; OesdkAddressSpaceMapping Mappings[OESDK_ADDRESS_SPACE_MAX_MAPPINGS]; bool Active; } OesdkAddressSpace;
OesdkStatus OesdkAddressSpaceCreate(uint64_t RootPhysicalAddress, OesdkAddressSpace *Space);
OesdkStatus OesdkAddressSpaceMap(OesdkAddressSpace *Space, uintptr_t VirtualAddress, uintptr_t PhysicalAddress, size_t PageCount, OesdkVirtualFlags Flags);
OesdkStatus OesdkAddressSpaceUnmap(OesdkAddressSpace *Space, uintptr_t VirtualAddress, size_t PageCount);
bool OesdkAddressSpaceQuery(const OesdkAddressSpace *Space, uintptr_t VirtualAddress, uintptr_t *PhysicalAddress, OesdkVirtualFlags *Flags);
bool OesdkUserRangeValidate(const OesdkAddressSpace *Space, uintptr_t Address, size_t Size, bool WriteRequested);
#ifdef __cplusplus
}
#endif
#endif
