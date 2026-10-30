
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/times.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>

/*
 * Minimal newlib syscall implementations for bare-metal ARM.
 *
 * Intended for single-core, single-threaded test execution.
 * Not a production-grade OS abstraction.
 */

#define HEAP_SIZE (256u * 1024u)

/* Align the heap for typical ARM C/C++ allocations. */
static union {
    max_align_t alignment;
    unsigned char data[HEAP_SIZE];
} heap_storage;

static size_t heap_used = 0;

/*
 * Called by exit() and abort().
 * A debugger such as TRACE32 can halt execution here.
 */
extern "C" __attribute__((noreturn))
void _exit(int status)
{
    (void)status;

    for (;;) {
        __asm__ volatile ("bkpt #0");
    }
}

/*
 * No filesystem.
 */
extern "C" int _open(const char *path, int flags, ...)
{
    (void)path;
    (void)flags;

    errno = ENOSYS;
    return -1;
}

/*
 * Heap allocation used by newlib malloc().
 * Memory cannot be returned to the heap via negative increments.
 */
extern "C" void *_sbrk(ptrdiff_t increment)
{
    if (increment < 0) {
        errno = ENOMEM;
        return (void *)-1;
    }

    size_t amount = (size_t)increment;
    const size_t alignment = _Alignof(max_align_t);

    if (amount > SIZE_MAX - (alignment - 1u)) {
        errno = ENOMEM;
        return (void *)-1;
    }

    size_t rounded =
        (amount + alignment - 1u) & ~(alignment - 1u);

    if (rounded > HEAP_SIZE - heap_used) {
        errno = ENOMEM;
        return (void *)-1;
    }

    void *result = &heap_storage.data[heap_used];
    heap_used += rounded;

    return result;
}

/*
 * No real-time clock available.
 */
extern "C" int _gettimeofday(struct timeval *tv, void *tz)
{
    (void)tv;
    (void)tz;

    errno = ENOSYS;
    return -1;
}


extern "C" clock_t _times(struct tms *buf)
{
    return 0;
}

extern "C" int _close (int __fildes)
{
    return 0;
}


extern "C" int _read (int __fd, void *__buf, size_t __nbyte)
{
    return 0;
}

extern "C" _off_t _lseek(int __fildes, _off_t __offset, int __whence )
{
    return 0;
}

extern "C" int _write (int __fd, const void *__buf, size_t __nbyte)
{
    return __nbyte;
}


extern "C" int _fstat (int __fd, struct stat *__sbuf )
{
    __sbuf->st_mode = S_IFCHR; /* character special device */
    return 0;
}

extern "C" int _getpid (void)
{
    return 123;
}


extern "C" int _kill (int a, int b)
{
    errno = EINVAL;
    return -1;
}

extern "C" int _isatty (int __fildes)
{
    return 1; /* IS a terminal */
}

