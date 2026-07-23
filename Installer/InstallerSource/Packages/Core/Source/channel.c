#include <oesdk/ipc.h>
#include <string.h>
OesdkStatus OesdkChannelInitialize(OesdkChannel*C){if(C==NULL)return OESDK_STATUS_INVALID_ARGUMENT;memset(C,0,sizeof(*C));return OESDK_STATUS_SUCCESS;}
