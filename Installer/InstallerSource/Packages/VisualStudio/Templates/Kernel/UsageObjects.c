#include "Usage.h"

void TemplateUsageObjects(void)
{
    OesdkChannel Channel;
    OesdkEndpoint Sender;
    OesdkEndpoint Receiver;
    OesdkIpcMessage Message = {0};
    OesdkIpcMessage Received = {0};
    OesdkCapability Parent;
    OesdkCapability Child;
    OesdkEvent Event;
    const OesdkSchedulerInformation *Scheduler = OesdkSchedulerInformationGet();
    OesdkStatus Status;
    uint32_t Object = 0x182U;

    Status = OesdkCapabilityCreate(&Object,
        OESDK_RIGHT_READ | OESDK_RIGHT_WRITE | OESDK_RIGHT_TRANSFER, &Parent);
    OESDK_ASSERT(OESDK_SUCCEEDED(Status));
    Status = OesdkCapabilityDerive(&Parent, OESDK_RIGHT_READ, &Child);
    OESDK_ASSERT(OESDK_SUCCEEDED(Status));
    OESDK_ASSERT(OesdkCapabilityAllows(&Child, OESDK_RIGHT_READ));

    OESDK_ASSERT(OESDK_SUCCEEDED(OesdkChannelInitialize(&Channel)));
    OESDK_ASSERT(OESDK_SUCCEEDED(OesdkEndpointBind(&Channel,
        OESDK_RIGHT_WRITE | OESDK_RIGHT_TRANSFER, &Sender)));
    OESDK_ASSERT(OESDK_SUCCEEDED(OesdkEndpointBind(&Channel, OESDK_RIGHT_READ, &Receiver)));
    Message.Tag = 1U;
    Message.Size = 4U;
    Message.Data[0] = 'O'; Message.Data[1] = 'E'; Message.Data[2] = 'S'; Message.Data[3] = 'D';
    OESDK_ASSERT(OESDK_SUCCEEDED(OesdkEndpointSend(&Sender, &Message)));
    OESDK_ASSERT(OESDK_SUCCEEDED(OesdkEndpointReceive(&Receiver, &Received)));

    OesdkEventInitialize(&Event, 1U);
    OesdkEventSignal(&Event);
    OESDK_ASSERT(OesdkEventConsume(&Event));

    if (Scheduler != NULL && Scheduler->Initialized) {
        kprintf("Scheduler: threads=%llu current=%llu quantum=%u ticks\n",
                (unsigned long long)Scheduler->ThreadCount,
                (unsigned long long)Scheduler->CurrentThreadId,
                (unsigned int)Scheduler->QuantumTicks);
    } else {
        kprintf("Scheduler/process/user-mode APIs are linked and await kernel policy setup.\n");
    }
    kprintf("Capability, channel, endpoint and event usage succeeded.\n");
}
