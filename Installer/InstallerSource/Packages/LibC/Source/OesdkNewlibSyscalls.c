#include <oesdk/libc.h>

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static const OesdkLibcHost *HostOrError(void)
{
    const OesdkLibcHost *Host = OesdkLibcGetHost();
    if (Host == NULL) errno = ENOSYS;
    return Host;
}

ssize_t _read(int fd, void *buffer, size_t count)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Read) { errno = ENOSYS; return -1; }
    ptrdiff_t r = h->Read(h->Context, fd, buffer, count);
    if (r < 0 && errno == 0) errno = EIO;
    return (ssize_t)r;
}

ssize_t _write(int fd, const void *buffer, size_t count)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Write) { errno = ENOSYS; return -1; }
    ptrdiff_t r = h->Write(h->Context, fd, buffer, count);
    if (r < 0 && errno == 0) errno = EIO;
    return (ssize_t)r;
}

int _open(const char *path, int flags, int mode)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Open) { errno = ENOSYS; return -1; }
    return h->Open(h->Context, path, flags, mode);
}

int _close(int fd)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Close) { errno = ENOSYS; return -1; }
    return h->Close(h->Context, fd);
}

off_t _lseek(int fd, off_t offset, int whence)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Seek) { errno = ENOSYS; return (off_t)-1; }
    return (off_t)h->Seek(h->Context, fd, (int64_t)offset, whence);
}

int _fstat(int fd, struct stat *st)
{
    const OesdkLibcHost *h = HostOrError();
    if (!st) { errno = EINVAL; return -1; }
    if (h && h->FileStat) return h->FileStat(h->Context, fd, st);
    if (fd >= 0 && fd <= 2) {
        st->st_mode = S_IFCHR;
        return 0;
    }
    errno = ENOSYS;
    return -1;
}

int _stat(const char *path, struct stat *st)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Stat) { errno = ENOSYS; return -1; }
    return h->Stat(h->Context, path, st);
}

int _isatty(int fd)
{
    const OesdkLibcHost *h = HostOrError();
    if (h && h->IsTerminal) return h->IsTerminal(h->Context, fd);
    return fd >= 0 && fd <= 2;
}

int _unlink(const char *path)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Unlink) { errno = ENOSYS; return -1; }
    return h->Unlink(h->Context, path);
}

int _rename(const char *old_path, const char *new_path)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Rename) { errno = ENOSYS; return -1; }
    return h->Rename(h->Context, old_path, new_path);
}

void *_sbrk(ptrdiff_t increment)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->HeapGrow) { errno = ENOMEM; return (void *)-1; }
    void *previous = h->HeapGrow(h->Context, increment);
    if (previous == (void *)-1 || previous == NULL) {
        errno = ENOMEM;
        return (void *)-1;
    }
    return previous;
}

int _getpid(void)
{
    const OesdkLibcHost *h = OesdkLibcGetHost();
    return (h && h->GetProcessId) ? h->GetProcessId(h->Context) : 1;
}

int _kill(int pid, int sig)
{
    const OesdkLibcHost *h = HostOrError();
    if (!h || !h->Kill) { errno = EINVAL; return -1; }
    return h->Kill(h->Context, pid, sig);
}

clock_t _times(struct tms *buffer)
{
    const OesdkLibcHost *h = HostOrError();
    int64_t sec = 0;
    int32_t nsec = 0;
    if (!h || !h->ClockGetTime || h->ClockGetTime(h->Context, 1, &sec, &nsec) != 0) {
        errno = ENOSYS;
        return (clock_t)-1;
    }
    if (buffer) {
        buffer->tms_utime = 0;
        buffer->tms_stime = 0;
        buffer->tms_cutime = 0;
        buffer->tms_cstime = 0;
    }
    return (clock_t)(sec * CLOCKS_PER_SEC + (nsec * CLOCKS_PER_SEC) / 1000000000);
}

int _gettimeofday(struct timeval *tv, void *tz)
{
    (void)tz;
    const OesdkLibcHost *h = HostOrError();
    int64_t sec = 0;
    int32_t nsec = 0;
    if (!tv) { errno = EINVAL; return -1; }
    if (!h || !h->ClockGetTime || h->ClockGetTime(h->Context, 0, &sec, &nsec) != 0) {
        errno = ENOSYS;
        return -1;
    }
    tv->tv_sec = (time_t)sec;
    tv->tv_usec = (suseconds_t)(nsec / 1000);
    return 0;
}

void _exit(int status)
{
    const OesdkLibcHost *h = OesdkLibcGetHost();
    if (h && h->ExitProcess) h->ExitProcess(h->Context, status);
    for (;;) { __asm__ volatile ("hlt"); }
}

/* No process image replacement or child processes are supplied by the
   baseline OESDK ABI. They fail explicitly rather than pretending success. */
int _execve(const char *name, char *const argv[], char *const env[])
{
    (void)name; (void)argv; (void)env; errno = ENOSYS; return -1;
}

int _fork(void)
{
    errno = ENOSYS;
    return -1;
}

int _link(const char *old_path, const char *new_path)
{
    (void)old_path; (void)new_path; errno = ENOSYS; return -1;
}

int _wait(int *status)
{
    (void)status; errno = ECHILD; return -1;
}
