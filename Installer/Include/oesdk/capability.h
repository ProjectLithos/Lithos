#ifndef OESDK_CAPABILITY_H
#define OESDK_CAPABILITY_H
#include <stdbool.h>
#include <stdint.h>
#include <oesdk/status.h>
typedef uint64_t OesdkRights;
#define OESDK_RIGHT_READ (UINT64_C(1)<<0)
#define OESDK_RIGHT_WRITE (UINT64_C(1)<<1)
#define OESDK_RIGHT_EXECUTE (UINT64_C(1)<<2)
#define OESDK_RIGHT_SIGNAL (UINT64_C(1)<<3)
#define OESDK_RIGHT_TRANSFER (UINT64_C(1)<<4)
#define OESDK_RIGHT_DUPLICATE (UINT64_C(1)<<5)
#define OESDK_RIGHT_MANAGE (UINT64_C(1)<<6)
typedef struct OesdkCapability { uint64_t Id; void *Object; OesdkRights Rights; bool Active; } OesdkCapability;
OesdkStatus OesdkCapabilityCreate(void *Object,OesdkRights Rights,OesdkCapability *Capability);
OesdkStatus OesdkCapabilityDerive(const OesdkCapability *Parent,OesdkRights RequestedRights,OesdkCapability *Child);
bool OesdkCapabilityAllows(const OesdkCapability *Capability,OesdkRights RequiredRights);
#endif
