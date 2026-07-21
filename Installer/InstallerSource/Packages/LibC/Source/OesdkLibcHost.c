#include <oesdk/libc.h>
#include <errno.h>
#include <string.h>

static OesdkLibcHost OesdkHost;
static int OesdkHostInstalled;

int OesdkLibcInstallHost(const OesdkLibcHost *Host)
{
    if (Host == NULL || Host->AbiVersion != OESDK_LIBC_ABI_VERSION ||
        Host->Write == NULL || Host->HeapGrow == NULL || Host->ExitProcess == NULL) {
        errno = EINVAL;
        return -1;
    }

    memcpy(&OesdkHost, Host, sizeof(OesdkHost));
    OesdkHostInstalled = 1;
    return 0;
}

const OesdkLibcHost *OesdkLibcGetHost(void)
{
    return OesdkHostInstalled ? &OesdkHost : NULL;
}

void OesdkLibcInitialize(void)
{
    /* Newlib initialises its global reentrancy object lazily.
       The host must be installed before any libc call which performs I/O,
       allocation, locking, time access, process control, or file access. */
}
