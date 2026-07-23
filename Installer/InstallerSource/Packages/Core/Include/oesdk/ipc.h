#ifndef OESDK_IPC_H
#define OESDK_IPC_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <oesdk/capability.h>
#include <oesdk/status.h>
#define OESDK_CHANNEL_DEPTH 16U
#define OESDK_IPC_PAYLOAD_MAX 256U
typedef struct OesdkIpcMessage { uint64_t Tag; size_t Size; uint8_t Data[OESDK_IPC_PAYLOAD_MAX]; OesdkCapability TransferredCapability; bool HasCapability; } OesdkIpcMessage;
typedef struct OesdkChannel { OesdkIpcMessage Queue[OESDK_CHANNEL_DEPTH]; size_t ReadIndex,WriteIndex,Count; } OesdkChannel;
typedef struct OesdkEndpoint { OesdkChannel *Channel; OesdkRights Rights; } OesdkEndpoint;
OesdkStatus OesdkChannelInitialize(OesdkChannel *Channel);
OesdkStatus OesdkEndpointBind(OesdkChannel *Channel,OesdkRights Rights,OesdkEndpoint *Endpoint);
OesdkStatus OesdkEndpointSend(OesdkEndpoint *Endpoint,const OesdkIpcMessage *Message);
OesdkStatus OesdkEndpointReceive(OesdkEndpoint *Endpoint,OesdkIpcMessage *Message);
#endif
