#include "Usage.h"

void TemplateUsageCpu(void)
{
    const OesdkCpuFeatures *Features = OesdkCpuFeaturesGet();
    const OesdkGdtInformation *Gdt = OesdkGdtInformationGet();
    const OesdkIdtInformation *Idt = OesdkIdtInformationGet();
    const OesdkSmpInformation *Smp = OesdkSmpInformationGet();
    OesdkCpuRegisters Registers;

    OESDK_ASSERT(Features != NULL);
    OesdkCpuQuery(0U, 0U, &Registers);
    kprintf("CPU vendor=%s long-mode=%u NX=%u APIC=%u x2APIC=%u syscall=%u\n",
            Features->Vendor, Features->LongMode ? 1U : 0U, Features->Nx ? 1U : 0U,
            Features->Apic ? 1U : 0U, Features->X2Apic ? 1U : 0U,
            Features->Syscall ? 1U : 0U);

    OESDK_ASSERT(OesdkGdtIsInitialized());
    OESDK_ASSERT(Gdt != NULL && Gdt->Initialized);
    OESDK_ASSERT(Idt != NULL && Idt->Initialized);
    kprintf("GDT descriptors=%u TSS=0x%x; IDT vectors=%u; page fault=%s\n",
            (unsigned int)Gdt->DescriptorCount, (unsigned int)Gdt->TaskStateSelector,
            (unsigned int)Idt->InstalledVectors, OesdkExceptionName(14U));

    if (Smp != NULL && Smp->Initialized) {
        kprintf("SMP processors: discovered=%u enabled=%u online=%u current-apic=%u bsp=%u\n",
                (unsigned int)Smp->DiscoveredProcessorCount,
                (unsigned int)Smp->EnabledProcessorCount,
                (unsigned int)Smp->OnlineProcessorCount,
                (unsigned int)OesdkSmpCurrentApicId(),
                OesdkSmpIsBootstrapProcessor() ? 1U : 0U);
    } else {
        kprintf("SMP startup is available but awaits firmware CPU topology.\n");
    }
}
