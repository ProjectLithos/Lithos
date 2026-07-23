#include <stddef.h>
#include <oesdk/interrupt_controller.h>
#include <oesdk/cpu.h>

#define PIC1_COMMAND 0x20U
#define PIC1_DATA    0x21U
#define PIC2_COMMAND 0xA0U
#define PIC2_DATA    0xA1U
#define PIC_EOI      0x20U
#define ICW1_INIT    0x10U
#define ICW1_ICW4    0x01U
#define ICW4_8086    0x01U

#define IA32_APIC_BASE_MSR 0x1BU
#define IA32_APIC_BASE_ENABLE (UINT64_C(1) << 11U)
#define IA32_APIC_BASE_X2APIC (UINT64_C(1) << 10U)
#define X2APIC_ID_MSR  0x802U
#define X2APIC_SVR_MSR 0x80FU
#define X2APIC_EOI_MSR 0x80BU
#define APIC_ID_OFFSET  0x020U
#define APIC_EOI_OFFSET 0x0B0U
#define APIC_SVR_OFFSET 0x0F0U
#define APIC_SVR_ENABLE (1U << 8U)
#define APIC_SPURIOUS_VECTOR 0xFFU
#define IOAPIC_REGSEL 0x00U
#define IOAPIC_WINDOW 0x10U
#define IOAPIC_REDTBL 0x10U

static OesdkInterruptControllerInformation State;
static uint16_t PicMask = UINT16_C(0xFFFF);

static void Out8(uint16_t Port, uint8_t Value) { __asm__ volatile("outb %0, %1" : : "a"(Value), "Nd"(Port)); }
static uint8_t In8(uint16_t Port) { uint8_t Value; __asm__ volatile("inb %1, %0" : "=a"(Value) : "Nd"(Port)); return Value; }
static uint64_t ReadMsr(uint32_t Register) { uint32_t Low, High; __asm__ volatile("rdmsr" : "=a"(Low), "=d"(High) : "c"(Register)); return ((uint64_t)High << 32U) | Low; }
static void WriteMsr(uint32_t Register, uint64_t Value) { __asm__ volatile("wrmsr" : : "c"(Register), "a"((uint32_t)Value), "d"((uint32_t)(Value >> 32U)) : "memory"); }
static uint32_t MmioRead32(uintptr_t Address) { return *(volatile uint32_t *)Address; }
static void MmioWrite32(uintptr_t Address, uint32_t Value) { *(volatile uint32_t *)Address = Value; }
static void IoApicWrite(uint8_t Register, uint32_t Value) { MmioWrite32(State.IoApicBase + IOAPIC_REGSEL, Register); MmioWrite32(State.IoApicBase + IOAPIC_WINDOW, Value); }

static void UpdatePicMask(void)
{
    Out8(PIC1_DATA, (uint8_t)(PicMask & UINT16_C(0xFF)));
    Out8(PIC2_DATA, (uint8_t)(PicMask >> 8U));
}

OesdkStatus OesdkInterruptControllerInitializePic(uint8_t VectorBase)
{
    uint8_t MasterMask;
    uint8_t SlaveMask;
    if (VectorBase < 32U || VectorBase > 240U || (VectorBase & 7U) != 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    MasterMask = In8(PIC1_DATA); SlaveMask = In8(PIC2_DATA);
    Out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); Out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    Out8(PIC1_DATA, VectorBase); Out8(PIC2_DATA, (uint8_t)(VectorBase + 8U));
    Out8(PIC1_DATA, 4U); Out8(PIC2_DATA, 2U);
    Out8(PIC1_DATA, ICW4_8086); Out8(PIC2_DATA, ICW4_8086);
    (void)MasterMask; (void)SlaveMask;
    PicMask = UINT16_C(0xFFFF); UpdatePicMask();
    State.Type = OesdkInterruptControllerPic;
    State.VectorBase = VectorBase;
    State.LocalApicBase = 0U; State.IoApicBase = 0U; State.LocalApicId = 0U;
    State.IoApicAvailable = false; State.Initialized = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkInterruptControllerInitializeLocalApic(uintptr_t LocalApicBase)
{
    const OesdkCpuFeatures *Features = OesdkCpuFeaturesGet();
    uint64_t ApicBase;
    if (Features == NULL || !Features->Apic) return OESDK_STATUS_NOT_SUPPORTED;
    if (LocalApicBase == 0U || (LocalApicBase & 0xFFFU) != 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    ApicBase = ReadMsr(IA32_APIC_BASE_MSR);
    WriteMsr(IA32_APIC_BASE_MSR, (ApicBase & UINT64_C(0xFFF)) | (uint64_t)LocalApicBase | IA32_APIC_BASE_ENABLE);
    MmioWrite32(LocalApicBase + APIC_SVR_OFFSET, APIC_SVR_ENABLE | APIC_SPURIOUS_VECTOR);
    State.Type = OesdkInterruptControllerLocalApic; State.LocalApicBase = LocalApicBase;
    State.LocalApicId = (uint8_t)(MmioRead32(LocalApicBase + APIC_ID_OFFSET) >> 24U);
    State.VectorBase = OESDK_PIC_VECTOR_BASE; State.Initialized = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkInterruptControllerInitializeX2Apic(void)
{
    const OesdkCpuFeatures *Features = OesdkCpuFeaturesGet();
    uint64_t ApicBase;
    if (Features == NULL || !Features->X2Apic) return OESDK_STATUS_NOT_SUPPORTED;
    ApicBase = ReadMsr(IA32_APIC_BASE_MSR);
    WriteMsr(IA32_APIC_BASE_MSR, ApicBase | IA32_APIC_BASE_ENABLE | IA32_APIC_BASE_X2APIC);
    WriteMsr(X2APIC_SVR_MSR, APIC_SVR_ENABLE | APIC_SPURIOUS_VECTOR);
    State.Type = OesdkInterruptControllerX2Apic; State.LocalApicBase = 0U;
    State.LocalApicId = (uint8_t)ReadMsr(X2APIC_ID_MSR); State.VectorBase = OESDK_PIC_VECTOR_BASE; State.Initialized = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkInterruptControllerAttachIoApic(uintptr_t IoApicBase)
{
    if (!State.Initialized) return OESDK_STATUS_NOT_FOUND;
    if (IoApicBase == 0U || (IoApicBase & 0xFFFU) != 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    State.IoApicBase = IoApicBase; State.IoApicAvailable = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkInterruptControllerRouteIrq(uint8_t Irq, uint8_t Vector, uint32_t DestinationApicId, uint64_t Flags)
{
    uint64_t Entry;
    if (!State.Initialized || !State.IoApicAvailable) return OESDK_STATUS_NOT_FOUND;
    if (Vector < 32U) return OESDK_STATUS_INVALID_ARGUMENT;
    Entry = (uint64_t)Vector | (Flags & (OESDK_IOAPIC_POLARITY_LOW | OESDK_IOAPIC_TRIGGER_LEVEL | OESDK_IOAPIC_MASKED));
    Entry |= ((uint64_t)(DestinationApicId & 0xFFU) << 56U);
    IoApicWrite((uint8_t)(IOAPIC_REDTBL + Irq * 2U), (uint32_t)Entry);
    IoApicWrite((uint8_t)(IOAPIC_REDTBL + Irq * 2U + 1U), (uint32_t)(Entry >> 32U));
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkInterruptControllerMaskIrq(uint8_t Irq)
{
    if (!State.Initialized || Irq >= OESDK_IRQ_COUNT) return OESDK_STATUS_INVALID_ARGUMENT;
    if (State.Type != OesdkInterruptControllerPic) return OESDK_STATUS_NOT_SUPPORTED;
    PicMask |= (uint16_t)(UINT16_C(1) << Irq); UpdatePicMask(); return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkInterruptControllerUnmaskIrq(uint8_t Irq)
{
    if (!State.Initialized || Irq >= OESDK_IRQ_COUNT) return OESDK_STATUS_INVALID_ARGUMENT;
    if (State.Type != OesdkInterruptControllerPic) return OESDK_STATUS_NOT_SUPPORTED;
    if (Irq >= 8U) PicMask &= (uint16_t)~(UINT16_C(1) << 2U);
    PicMask &= (uint16_t)~(UINT16_C(1) << Irq); UpdatePicMask(); return OESDK_STATUS_SUCCESS;
}

void OesdkInterruptControllerEndOfInterrupt(uint8_t Vector)
{
    if (!State.Initialized) return;
    if (State.Type == OesdkInterruptControllerPic) {
        if (Vector < State.VectorBase || Vector >= (uint8_t)(State.VectorBase + OESDK_IRQ_COUNT)) return;
        if (Vector >= (uint8_t)(State.VectorBase + 8U)) Out8(PIC2_COMMAND, PIC_EOI);
        Out8(PIC1_COMMAND, PIC_EOI);
    } else if (State.Type == OesdkInterruptControllerLocalApic) {
        MmioWrite32(State.LocalApicBase + APIC_EOI_OFFSET, 0U);
    } else if (State.Type == OesdkInterruptControllerX2Apic) {
        WriteMsr(X2APIC_EOI_MSR, 0U);
    }
}

bool OesdkInterruptControllerOwnsVector(uint8_t Vector)
{
    if (!State.Initialized) return false;
    if (State.Type == OesdkInterruptControllerPic) return Vector >= State.VectorBase && Vector < (uint8_t)(State.VectorBase + OESDK_IRQ_COUNT);
    return Vector >= 32U;
}

const OesdkInterruptControllerInformation *OesdkInterruptControllerInformationGet(void) { return State.Initialized ? &State : NULL; }
