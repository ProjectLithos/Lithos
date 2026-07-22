#ifndef OESDK_STATUS_H
#define OESDK_STATUS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t OesdkStatus;

enum {
    OESDK_STATUS_SUCCESS = 0,
    OESDK_STATUS_INVALID_ARGUMENT = -1,
    OESDK_STATUS_NOT_SUPPORTED = -2,
    OESDK_STATUS_OUT_OF_MEMORY = -3,
    OESDK_STATUS_NOT_FOUND = -4,
    OESDK_STATUS_ALREADY_EXISTS = -5,
    OESDK_STATUS_BUSY = -6,
    OESDK_STATUS_TIMEOUT = -7,
    OESDK_STATUS_CORRUPT = -8,
    OESDK_STATUS_ACCESS_DENIED = -9,
    OESDK_STATUS_BUFFER_TOO_SMALL = -10,
    OESDK_STATUS_INVALID_STATE = -11,
    OESDK_STATUS_IO_ERROR = -12,
    OESDK_STATUS_INTERRUPTED = -13,
    OESDK_STATUS_OVERFLOW = -14,
    OESDK_STATUS_UNDERFLOW = -15
};

#define OESDK_SUCCEEDED(Status) ((OesdkStatus)(Status) >= OESDK_STATUS_SUCCESS)
#define OESDK_FAILED(Status) ((OesdkStatus)(Status) < OESDK_STATUS_SUCCESS)

const char *OesdkStatusName(OesdkStatus Status);
const char *OesdkStatusDescription(OesdkStatus Status);

#ifdef __cplusplus
}
#endif

#endif
