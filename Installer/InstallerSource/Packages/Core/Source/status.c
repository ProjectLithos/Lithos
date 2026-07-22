#include <oesdk/status.h>
#include <stddef.h>

typedef struct OesdkStatusTextEntry
{
    OesdkStatus Status;
    const char *Name;
    const char *Description;
} OesdkStatusTextEntry;

static const OesdkStatusTextEntry OesdkStatusText[] =
{
    { OESDK_STATUS_SUCCESS, "OESDK_STATUS_SUCCESS", "The operation completed successfully." },
    { OESDK_STATUS_INVALID_ARGUMENT, "OESDK_STATUS_INVALID_ARGUMENT", "An argument was invalid." },
    { OESDK_STATUS_NOT_SUPPORTED, "OESDK_STATUS_NOT_SUPPORTED", "The requested operation is not supported." },
    { OESDK_STATUS_OUT_OF_MEMORY, "OESDK_STATUS_OUT_OF_MEMORY", "The requested memory could not be allocated." },
    { OESDK_STATUS_NOT_FOUND, "OESDK_STATUS_NOT_FOUND", "The requested object was not found." },
    { OESDK_STATUS_ALREADY_EXISTS, "OESDK_STATUS_ALREADY_EXISTS", "The requested object already exists." },
    { OESDK_STATUS_BUSY, "OESDK_STATUS_BUSY", "The object or subsystem is busy." },
    { OESDK_STATUS_TIMEOUT, "OESDK_STATUS_TIMEOUT", "The operation did not complete before its deadline." },
    { OESDK_STATUS_CORRUPT, "OESDK_STATUS_CORRUPT", "Data or state failed validation." }
};

static const OesdkStatusTextEntry *OesdkStatusFind(OesdkStatus Status)
{
    size_t Index;

    for (Index = 0U; Index < sizeof(OesdkStatusText) / sizeof(OesdkStatusText[0]); ++Index)
    {
        if (OesdkStatusText[Index].Status == Status)
        {
            return &OesdkStatusText[Index];
        }
    }

    return NULL;
}

const char *OesdkStatusName(OesdkStatus Status)
{
    const OesdkStatusTextEntry *Entry = OesdkStatusFind(Status);
    return Entry != NULL ? Entry->Name : "OESDK_STATUS_UNKNOWN";
}

const char *OesdkStatusDescription(OesdkStatus Status)
{
    const OesdkStatusTextEntry *Entry = OesdkStatusFind(Status);
    return Entry != NULL ? Entry->Description : "An unknown OESDK status value was returned.";
}
