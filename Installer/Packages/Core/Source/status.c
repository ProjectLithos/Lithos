#include <oesdk/status.h>
#include <stddef.h>

typedef struct OesdkStatusTextEntry {
    OesdkStatus Status;
    const char *Name;
    const char *Description;
} OesdkStatusTextEntry;

static const OesdkStatusTextEntry StatusText[] = {
    { OESDK_STATUS_SUCCESS, "OESDK_STATUS_SUCCESS", "The operation completed successfully." },
    { OESDK_STATUS_INVALID_ARGUMENT, "OESDK_STATUS_INVALID_ARGUMENT", "An argument was invalid." },
    { OESDK_STATUS_NOT_SUPPORTED, "OESDK_STATUS_NOT_SUPPORTED", "The requested operation is not supported." },
    { OESDK_STATUS_OUT_OF_MEMORY, "OESDK_STATUS_OUT_OF_MEMORY", "The requested memory could not be allocated." },
    { OESDK_STATUS_NOT_FOUND, "OESDK_STATUS_NOT_FOUND", "The requested object was not found." },
    { OESDK_STATUS_ALREADY_EXISTS, "OESDK_STATUS_ALREADY_EXISTS", "The requested object already exists." },
    { OESDK_STATUS_BUSY, "OESDK_STATUS_BUSY", "The object or subsystem is busy." },
    { OESDK_STATUS_TIMEOUT, "OESDK_STATUS_TIMEOUT", "The operation did not complete before its deadline." },
    { OESDK_STATUS_CORRUPT, "OESDK_STATUS_CORRUPT", "Data or state failed validation." },
    { OESDK_STATUS_ACCESS_DENIED, "OESDK_STATUS_ACCESS_DENIED", "The caller does not have the required access." },
    { OESDK_STATUS_BUFFER_TOO_SMALL, "OESDK_STATUS_BUFFER_TOO_SMALL", "The supplied buffer is too small." },
    { OESDK_STATUS_INVALID_STATE, "OESDK_STATUS_INVALID_STATE", "The operation is invalid in the current state." },
    { OESDK_STATUS_IO_ERROR, "OESDK_STATUS_IO_ERROR", "An input/output operation failed." },
    { OESDK_STATUS_INTERRUPTED, "OESDK_STATUS_INTERRUPTED", "The operation was interrupted before completion." },
    { OESDK_STATUS_OVERFLOW, "OESDK_STATUS_OVERFLOW", "An arithmetic or capacity overflow was detected." },
    { OESDK_STATUS_UNDERFLOW, "OESDK_STATUS_UNDERFLOW", "An arithmetic or capacity underflow was detected." }
};

static const OesdkStatusTextEntry *find_status(OesdkStatus Status) {
    size_t Index;
    for (Index = 0U; Index < sizeof(StatusText) / sizeof(StatusText[0]); ++Index) {
        if (StatusText[Index].Status == Status) return &StatusText[Index];
    }
    return NULL;
}

const char *OesdkStatusName(OesdkStatus Status) {
    const OesdkStatusTextEntry *Entry = find_status(Status);
    return Entry != NULL ? Entry->Name : "OESDK_STATUS_UNKNOWN";
}

const char *OesdkStatusDescription(OesdkStatus Status) {
    const OesdkStatusTextEntry *Entry = find_status(Status);
    return Entry != NULL ? Entry->Description : "An unknown OESDK status value was returned.";
}
