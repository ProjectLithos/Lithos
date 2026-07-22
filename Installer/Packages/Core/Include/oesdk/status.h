#ifndef OESDK_STATUS_H
#define OESDK_STATUS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t OesdkStatus;

#define OESDK_STATUS_SUCCESS              ((OesdkStatus)0)
#define OESDK_STATUS_INVALID_ARGUMENT     ((OesdkStatus)-1)
#define OESDK_STATUS_NOT_SUPPORTED        ((OesdkStatus)-2)
#define OESDK_STATUS_OUT_OF_MEMORY        ((OesdkStatus)-3)
#define OESDK_STATUS_NOT_FOUND            ((OesdkStatus)-4)
#define OESDK_STATUS_ALREADY_EXISTS       ((OesdkStatus)-5)
#define OESDK_STATUS_BUSY                 ((OesdkStatus)-6)
#define OESDK_STATUS_TIMEOUT              ((OesdkStatus)-7)
#define OESDK_STATUS_CORRUPT              ((OesdkStatus)-8)

#define OESDK_SUCCEEDED(Status) ((OesdkStatus)(Status) >= OESDK_STATUS_SUCCESS)
#define OESDK_FAILED(Status)    ((OesdkStatus)(Status) < OESDK_STATUS_SUCCESS)

const char *OesdkStatusName(OesdkStatus Status);
const char *OesdkStatusDescription(OesdkStatus Status);

#ifdef __cplusplus
}
#endif

#endif
