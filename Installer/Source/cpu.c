#include <oesdk/cpu.h>
#include <stddef.h>
#include <stdint.h>

#define OESDK_CPUID_EXTENDED_BASE 0x80000000U
#define OESDK_RFLAGS_INTERRUPT_ENABLE (1ULL << 9)

static OesdkCpuFeatures CpuFeatures;
static bool CpuFeaturesInitialized;

void OesdkCpuQuery(uint32_t Leaf, uint32_t Subleaf, OesdkCpuRegisters *Registers)
{
    uint32_t Eax;
    uint32_t Ebx;
    uint32_t Ecx;
    uint32_t Edx;

    if (Registers == NULL) {
        return;
    }

    __asm__ volatile (
        "cpuid"
        : "=a" (Eax), "=b" (Ebx), "=c" (Ecx), "=d" (Edx)
        : "a" (Leaf), "c" (Subleaf));

    Registers->Eax = Eax;
    Registers->Ebx = Ebx;
    Registers->Ecx = Ecx;
    Registers->Edx = Edx;
}

static void copy_u32(char *Destination, uint32_t Value)
{
    Destination[0] = (char)(Value & 0xFFU);
    Destination[1] = (char)((Value >> 8) & 0xFFU);
    Destination[2] = (char)((Value >> 16) & 0xFFU);
    Destination[3] = (char)((Value >> 24) & 0xFFU);
}

bool OesdkCpuInitialize(void)
{
    OesdkCpuRegisters Basic;
    OesdkCpuRegisters Extended;

    OesdkCpuQuery(0U, 0U, &Basic);
    CpuFeatures.MaximumBasicLeaf = Basic.Eax;
    copy_u32(&CpuFeatures.Vendor[0], Basic.Ebx);
    copy_u32(&CpuFeatures.Vendor[4], Basic.Edx);
    copy_u32(&CpuFeatures.Vendor[8], Basic.Ecx);
    CpuFeatures.Vendor[12] = '\0';

    OesdkCpuQuery(OESDK_CPUID_EXTENDED_BASE, 0U, &Extended);
    CpuFeatures.MaximumExtendedLeaf = Extended.Eax;

    if (CpuFeatures.MaximumBasicLeaf >= 1U) {
        OesdkCpuQuery(1U, 0U, &Basic);
        CpuFeatures.Tsc = (Basic.Edx & (1U << 4)) != 0U;
        CpuFeatures.Apic = (Basic.Edx & (1U << 9)) != 0U;
        CpuFeatures.X2Apic = (Basic.Ecx & (1U << 21)) != 0U;
    }

    if (CpuFeatures.MaximumExtendedLeaf >= 0x80000001U) {
        OesdkCpuQuery(0x80000001U, 0U, &Extended);
        CpuFeatures.Syscall = (Extended.Edx & (1U << 11)) != 0U;
        CpuFeatures.Nx = (Extended.Edx & (1U << 20)) != 0U;
        CpuFeatures.Page1GiB = (Extended.Edx & (1U << 26)) != 0U;
        CpuFeatures.LongMode = (Extended.Edx & (1U << 29)) != 0U;
    }

    if (CpuFeatures.MaximumExtendedLeaf >= 0x80000007U) {
        OesdkCpuQuery(0x80000007U, 0U, &Extended);
        CpuFeatures.InvariantTsc = (Extended.Edx & (1U << 8)) != 0U;
    }

    CpuFeaturesInitialized = true;
    return CpuFeatures.LongMode;
}

const OesdkCpuFeatures *OesdkCpuFeaturesGet(void)
{
    return CpuFeaturesInitialized ? &CpuFeatures : NULL;
}

void OesdkCpuDisableInterrupts(void)
{
    __asm__ volatile ("cli" ::: "memory");
}

void OesdkCpuEnableInterrupts(void)
{
    __asm__ volatile ("sti" ::: "memory");
}

uint64_t OesdkCpuReadFlags(void)
{
    uint64_t Flags;
    __asm__ volatile ("pushfq; popq %0" : "=r" (Flags) :: "memory");
    return Flags;
}

uint64_t OesdkCpuInterruptStateSave(void)
{
    uint64_t Flags = OesdkCpuReadFlags();
    OesdkCpuDisableInterrupts();
    return Flags;
}

void OesdkCpuInterruptStateRestore(uint64_t Flags)
{
    if ((Flags & OESDK_RFLAGS_INTERRUPT_ENABLE) != 0U) {
        OesdkCpuEnableInterrupts();
    } else {
        OesdkCpuDisableInterrupts();
    }
}

void OesdkCpuHalt(void)
{
    __asm__ volatile ("hlt");
}

void OesdkCpuPause(void)
{
    __asm__ volatile ("pause");
}

uint64_t OesdkCpuReadCr0(void)
{
    uint64_t Value;
    __asm__ volatile ("mov %%cr0, %0" : "=r" (Value));
    return Value;
}

uint64_t OesdkCpuReadCr2(void)
{
    uint64_t Value;
    __asm__ volatile ("mov %%cr2, %0" : "=r" (Value));
    return Value;
}

uint64_t OesdkCpuReadCr3(void)
{
    uint64_t Value;
    __asm__ volatile ("mov %%cr3, %0" : "=r" (Value));
    return Value;
}

uint64_t OesdkCpuReadCr4(void)
{
    uint64_t Value;
    __asm__ volatile ("mov %%cr4, %0" : "=r" (Value));
    return Value;
}

void OesdkCpuWriteCr3(uint64_t Value)
{
    __asm__ volatile ("mov %0, %%cr3" :: "r" (Value) : "memory");
}

void OesdkCpuInvalidatePage(uintptr_t VirtualAddress)
{
    __asm__ volatile ("invlpg (%0)" :: "r" (VirtualAddress) : "memory");
}
