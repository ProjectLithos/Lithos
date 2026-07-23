#include <oesdk/libc.h>
#include <sys/lock.h>

static unsigned NextLockId = 1;

void __retarget_lock_init(_LOCK_T *lock) { *lock = (_LOCK_T)(uintptr_t)NextLockId++; }
void __retarget_lock_init_recursive(_LOCK_RECURSIVE_T *lock) { *lock = (_LOCK_RECURSIVE_T)(uintptr_t)NextLockId++; }
void __retarget_lock_close(_LOCK_T lock) { (void)lock; }
void __retarget_lock_close_recursive(_LOCK_RECURSIVE_T lock) { (void)lock; }

void __retarget_lock_acquire(_LOCK_T lock)
{
    const OesdkLibcHost *h = OesdkLibcGetHost();
    if (h && h->Lock) h->Lock(h->Context, (unsigned)(uintptr_t)lock);
}
void __retarget_lock_acquire_recursive(_LOCK_RECURSIVE_T lock)
{
    __retarget_lock_acquire((_LOCK_T)lock);
}
int __retarget_lock_try_acquire(_LOCK_T lock)
{
    __retarget_lock_acquire(lock);
    return 1;
}
int __retarget_lock_try_acquire_recursive(_LOCK_RECURSIVE_T lock)
{
    return __retarget_lock_try_acquire((_LOCK_T)lock);
}
void __retarget_lock_release(_LOCK_T lock)
{
    const OesdkLibcHost *h = OesdkLibcGetHost();
    if (h && h->Unlock) h->Unlock(h->Context, (unsigned)(uintptr_t)lock);
}
void __retarget_lock_release_recursive(_LOCK_RECURSIVE_T lock)
{
    __retarget_lock_release((_LOCK_T)lock);
}
