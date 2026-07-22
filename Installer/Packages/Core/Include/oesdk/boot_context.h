#ifndef OESDK_BOOT_CONTEXT_H
#define OESDK_BOOT_CONTEXT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define OESDK_MULTIBOOT1_BOOT_MAGIC 0x2BADB002U
#define OESDK_BOOT_CONTEXT_ABI_VERSION 1U

typedef enum OesdkBootProtocol {
    OesdkBootProtocolUnknown = 0,
    OesdkBootProtocolMultiboot1 = 1
} OesdkBootProtocol;

typedef struct OesdkBootContext {
    uint32_t StructureSize;
    uint32_t AbiVersion;
    OesdkBootProtocol BootProtocol;
    uint32_t BootMagic;
    uintptr_t BootInformationAddress;
    uintptr_t KernelPhysicalStart;
    uintptr_t KernelPhysicalEnd;
    uintptr_t KernelLoadEnd;
    uintptr_t BootstrapStackStart;
    uintptr_t BootstrapStackEnd;
    uint64_t AvailableMemoryBytes;
    bool MemorySizeAvailable;
} OesdkBootContext;

bool OesdkBootContextInitialize(uint32_t BootMagic, uintptr_t BootInformationAddress);
const OesdkBootContext *OesdkBootContextGet(void);

#endif
