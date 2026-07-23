#include <oesdk/timer.h>
#include <oesdk/cpu.h>
#include <oesdk/interrupt.h>

#define OESDK_PIT_CHANNEL0_PORT 0x40U
#define OESDK_PIT_COMMAND_PORT 0x43U
#define OESDK_PIT_MODE_RATE_GENERATOR 0x34U
#define OESDK_APIC_LVT_TIMER_OFFSET 0x320U
#define OESDK_APIC_INITIAL_COUNT_OFFSET 0x380U
#define OESDK_APIC_DIVIDE_OFFSET 0x3E0U
#define OESDK_APIC_LVT_PERIODIC (1U << 17U)
#define OESDK_APIC_LVT_MASKED (1U << 16U)
#define OESDK_X2APIC_LVT_TIMER_MSR 0x832U
#define OESDK_X2APIC_INITIAL_COUNT_MSR 0x838U
#define OESDK_X2APIC_DIVIDE_MSR 0x83EU
#define OESDK_HPET_CAPABILITIES_OFFSET 0x000U
#define OESDK_HPET_CONFIGURATION_OFFSET 0x010U
#define OESDK_HPET_MAIN_COUNTER_OFFSET 0x0F0U
#define OESDK_HPET_TIMER0_CONFIGURATION_OFFSET 0x100U
#define OESDK_HPET_TIMER0_COMPARATOR_OFFSET 0x108U
#define OESDK_HPET_ENABLE (UINT64_C(1) << 0U)
#define OESDK_HPET_TIMER_INTERRUPT_ENABLE (UINT64_C(1) << 2U)
#define OESDK_HPET_TIMER_PERIODIC_ENABLE (UINT64_C(1) << 3U)
#define OESDK_HPET_TIMER_PERIODIC_CAPABLE (UINT64_C(1) << 4U)
#define OESDK_HPET_TIMER_SET_COMPARATOR (UINT64_C(1) << 6U)

static volatile uint64_t OesdkTimerTickCounter;
static OesdkTimerInformation OesdkTimerState;

static void OesdkTimerOut8(uint16_t Port, uint8_t Value)
{
    __asm__ volatile("outb %0, %1" : : "a"(Value), "Nd"(Port));
}

static void OesdkTimerWriteMsr(uint32_t Register, uint64_t Value)
{
    __asm__ volatile("wrmsr" : : "c"(Register), "a"((uint32_t)Value), "d"((uint32_t)(Value >> 32U)) : "memory");
}

static void OesdkTimerMmioWrite32(uintptr_t Address, uint32_t Value)
{
    *(volatile uint32_t *)Address = Value;
}

static uint64_t OesdkTimerMmioRead64(uintptr_t Address)
{
    return *(volatile uint64_t *)Address;
}

static void OesdkTimerMmioWrite64(uintptr_t Address, uint64_t Value)
{
    *(volatile uint64_t *)Address = Value;
}

static void OesdkTimerInterrupt(OesdkInterruptFrame *Frame, void *Context)
{
    (void)Frame;
    (void)Context;
    ++OesdkTimerTickCounter;
}

static OesdkStatus OesdkTimerBegin(OesdkTimerSource Source, uint8_t Vector)
{
    OesdkStatus Status;
    if (Vector < OESDK_INTERRUPT_VECTOR_FIRST) return OESDK_STATUS_INVALID_ARGUMENT;
    if (OesdkTimerState.Initialized) return OESDK_STATUS_BUSY;
    Status = OesdkInterruptHandlerRegister(Vector, OesdkTimerInterrupt, NULL);
    if (OESDK_FAILED(Status)) return Status;
    OesdkTimerTickCounter = 0U;
    OesdkTimerState.Source = Source;
    OesdkTimerState.Vector = Vector;
    OesdkTimerState.Initialized = true;
    OesdkTimerState.Periodic = true;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkTimerInitializePit(uint32_t DesiredHz, uint8_t Vector)
{
    uint32_t Divisor;
    OesdkStatus Status;
    if (DesiredHz == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    Divisor = (OESDK_PIT_BASE_FREQUENCY + DesiredHz / 2U) / DesiredHz;
    if (Divisor == 0U || Divisor > 65535U) return OESDK_STATUS_NOT_SUPPORTED;
    Status = OesdkTimerBegin(OesdkTimerSourcePit, Vector);
    if (OESDK_FAILED(Status)) return Status;
    OesdkTimerOut8(OESDK_PIT_COMMAND_PORT, OESDK_PIT_MODE_RATE_GENERATOR);
    OesdkTimerOut8(OESDK_PIT_CHANNEL0_PORT, (uint8_t)(Divisor & 0xFFU));
    OesdkTimerOut8(OESDK_PIT_CHANNEL0_PORT, (uint8_t)((Divisor >> 8U) & 0xFFU));
    OesdkTimerState.RequestedHz = DesiredHz;
    OesdkTimerState.ActualHz = OESDK_PIT_BASE_FREQUENCY / Divisor;
    OesdkTimerState.Divisor = Divisor;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkTimerInitializeLocalApic(uintptr_t ApicBase, uint32_t InitialCount, uint32_t DivideConfiguration, uint8_t Vector)
{
    OesdkStatus Status;
    if (ApicBase == 0U || (ApicBase & 0xFFFU) != 0U || InitialCount == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    Status = OesdkTimerBegin(OesdkTimerSourceLocalApic, Vector);
    if (OESDK_FAILED(Status)) return Status;
    OesdkTimerMmioWrite32(ApicBase + OESDK_APIC_DIVIDE_OFFSET, DivideConfiguration);
    OesdkTimerMmioWrite32(ApicBase + OESDK_APIC_LVT_TIMER_OFFSET, (uint32_t)Vector | OESDK_APIC_LVT_PERIODIC);
    OesdkTimerMmioWrite32(ApicBase + OESDK_APIC_INITIAL_COUNT_OFFSET, InitialCount);
    OesdkTimerState.Divisor = DivideConfiguration;
    OesdkTimerState.HardwareBase = ApicBase;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkTimerInitializeX2Apic(uint32_t InitialCount, uint32_t DivideConfiguration, uint8_t Vector)
{
    const OesdkCpuFeatures *Features = OesdkCpuFeaturesGet();
    OesdkStatus Status;
    if (InitialCount == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    if (Features == NULL || !Features->X2Apic) return OESDK_STATUS_NOT_SUPPORTED;
    Status = OesdkTimerBegin(OesdkTimerSourceX2Apic, Vector);
    if (OESDK_FAILED(Status)) return Status;
    OesdkTimerWriteMsr(OESDK_X2APIC_DIVIDE_MSR, DivideConfiguration);
    OesdkTimerWriteMsr(OESDK_X2APIC_LVT_TIMER_MSR, (uint64_t)Vector | OESDK_APIC_LVT_PERIODIC);
    OesdkTimerWriteMsr(OESDK_X2APIC_INITIAL_COUNT_MSR, InitialCount);
    OesdkTimerState.Divisor = DivideConfiguration;
    return OESDK_STATUS_SUCCESS;
}

OesdkStatus OesdkTimerInitializeHpet(uintptr_t HpetBase, uint32_t DesiredHz, uint8_t Vector)
{
    uint64_t Capabilities;
    uint64_t PeriodFs;
    uint64_t Frequency;
    uint64_t Comparator;
    uint64_t TimerConfiguration;
    OesdkStatus Status;
    if (HpetBase == 0U || (HpetBase & 7U) != 0U || DesiredHz == 0U) return OESDK_STATUS_INVALID_ARGUMENT;
    Capabilities = OesdkTimerMmioRead64(HpetBase + OESDK_HPET_CAPABILITIES_OFFSET);
    PeriodFs = Capabilities >> 32U;
    if (PeriodFs == 0U) return OESDK_STATUS_CORRUPT;
    Frequency = UINT64_C(1000000000000000) / PeriodFs;
    Comparator = (Frequency + DesiredHz / 2U) / DesiredHz;
    if (Comparator == 0U) return OESDK_STATUS_NOT_SUPPORTED;
    TimerConfiguration = OesdkTimerMmioRead64(HpetBase + OESDK_HPET_TIMER0_CONFIGURATION_OFFSET);
    if ((TimerConfiguration & OESDK_HPET_TIMER_PERIODIC_CAPABLE) == 0U) return OESDK_STATUS_NOT_SUPPORTED;
    Status = OesdkTimerBegin(OesdkTimerSourceHpet, Vector);
    if (OESDK_FAILED(Status)) return Status;
    OesdkTimerMmioWrite64(HpetBase + OESDK_HPET_CONFIGURATION_OFFSET, 0U);
    OesdkTimerMmioWrite64(HpetBase + OESDK_HPET_MAIN_COUNTER_OFFSET, 0U);
    TimerConfiguration |= OESDK_HPET_TIMER_INTERRUPT_ENABLE | OESDK_HPET_TIMER_PERIODIC_ENABLE | OESDK_HPET_TIMER_SET_COMPARATOR;
    OesdkTimerMmioWrite64(HpetBase + OESDK_HPET_TIMER0_CONFIGURATION_OFFSET, TimerConfiguration);
    OesdkTimerMmioWrite64(HpetBase + OESDK_HPET_TIMER0_COMPARATOR_OFFSET, Comparator);
    OesdkTimerMmioWrite64(HpetBase + OESDK_HPET_TIMER0_COMPARATOR_OFFSET, Comparator);
    OesdkTimerMmioWrite64(HpetBase + OESDK_HPET_CONFIGURATION_OFFSET, OESDK_HPET_ENABLE);
    OesdkTimerState.RequestedHz = DesiredHz;
    OesdkTimerState.ActualHz = (uint32_t)(Frequency / Comparator);
    OesdkTimerState.Divisor = (uint32_t)(Comparator > UINT32_MAX ? UINT32_MAX : Comparator);
    OesdkTimerState.HardwareBase = HpetBase;
    return OESDK_STATUS_SUCCESS;
}

void OesdkTimerStop(void)
{
    if (!OesdkTimerState.Initialized) return;
    if (OesdkTimerState.Source == OesdkTimerSourceLocalApic) {
        OesdkTimerMmioWrite32(OesdkTimerState.HardwareBase + OESDK_APIC_LVT_TIMER_OFFSET,
                              OESDK_APIC_LVT_MASKED | OesdkTimerState.Vector);
        OesdkTimerMmioWrite32(OesdkTimerState.HardwareBase + OESDK_APIC_INITIAL_COUNT_OFFSET, 0U);
    } else if (OesdkTimerState.Source == OesdkTimerSourceX2Apic) {
        OesdkTimerWriteMsr(OESDK_X2APIC_LVT_TIMER_MSR, OESDK_APIC_LVT_MASKED | OesdkTimerState.Vector);
        OesdkTimerWriteMsr(OESDK_X2APIC_INITIAL_COUNT_MSR, 0U);
    } else if (OesdkTimerState.Source == OesdkTimerSourceHpet) {
        OesdkTimerMmioWrite64(OesdkTimerState.HardwareBase + OESDK_HPET_CONFIGURATION_OFFSET, 0U);
    }
    (void)OesdkInterruptHandlerUnregister(OesdkTimerState.Vector, OesdkTimerInterrupt);
    OesdkTimerState.Initialized = false;
    OesdkTimerState.Source = OesdkTimerSourceNone;
}

const OesdkTimerInformation *OesdkTimerInformationGet(void) { return &OesdkTimerState; }
uint64_t OesdkTimerTicks(void) { return OesdkTimerTickCounter; }
uint64_t OesdkTimerTicksToSeconds(uint64_t Ticks, uint32_t Hz) { return Hz == 0U ? 0U : Ticks / Hz; }

uint64_t OesdkTimerTicksToMilliseconds(uint64_t Ticks, uint32_t Hz)
{
    uint64_t Seconds;
    uint64_t Remainder;
    if (Hz == 0U) return 0U;
    Seconds = Ticks / Hz;
    Remainder = Ticks % Hz;
    if (Seconds > UINT64_MAX / 1000U) return UINT64_MAX;
    return Seconds * 1000U + (Remainder * 1000U) / Hz;
}

uint64_t OesdkTimerTicksToNanoseconds(uint64_t Ticks, uint32_t Hz)
{
    uint64_t Seconds;
    uint64_t Remainder;
    if (Hz == 0U) return 0U;
    Seconds = Ticks / Hz;
    Remainder = Ticks % Hz;
    if (Seconds > UINT64_MAX / OESDK_TIMER_NANOSECONDS_PER_SECOND) return UINT64_MAX;
    return Seconds * OESDK_TIMER_NANOSECONDS_PER_SECOND
         + (Remainder * OESDK_TIMER_NANOSECONDS_PER_SECOND) / Hz;
}
