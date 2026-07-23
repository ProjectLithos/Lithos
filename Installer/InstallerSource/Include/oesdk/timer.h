#ifndef OESDK_TIMER_H
#define OESDK_TIMER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OESDK_PIT_BASE_FREQUENCY 1193182U
#define OESDK_TIMER_DEFAULT_VECTOR 32U
#define OESDK_TIMER_NANOSECONDS_PER_SECOND UINT64_C(1000000000)

typedef enum OesdkTimerSource
{
    OesdkTimerSourceNone = 0,
    OesdkTimerSourcePit,
    OesdkTimerSourceLocalApic,
    OesdkTimerSourceX2Apic,
    OesdkTimerSourceHpet
} OesdkTimerSource;

typedef struct OesdkTimerInformation
{
    OesdkTimerSource Source;
    uint32_t RequestedHz;
    uint32_t ActualHz;
    uint32_t Divisor;
    uint8_t Vector;
    uint64_t TickCount;
    uintptr_t HardwareBase;
    bool Initialized;
    bool Periodic;
} OesdkTimerInformation;

OesdkStatus OesdkTimerInitializePit(uint32_t DesiredHz, uint8_t Vector);
OesdkStatus OesdkTimerInitializeLocalApic(uintptr_t ApicBase, uint32_t InitialCount, uint32_t DivideConfiguration, uint8_t Vector);
OesdkStatus OesdkTimerInitializeX2Apic(uint32_t InitialCount, uint32_t DivideConfiguration, uint8_t Vector);
OesdkStatus OesdkTimerInitializeHpet(uintptr_t HpetBase, uint32_t DesiredHz, uint8_t Vector);
void OesdkTimerStop(void);
const OesdkTimerInformation *OesdkTimerInformationGet(void);
uint64_t OesdkTimerTicks(void);
uint64_t OesdkTimerTicksToSeconds(uint64_t Ticks, uint32_t Hz);
uint64_t OesdkTimerTicksToMilliseconds(uint64_t Ticks, uint32_t Hz);
uint64_t OesdkTimerTicksToNanoseconds(uint64_t Ticks, uint32_t Hz);

#ifdef __cplusplus
}
#endif

#endif
