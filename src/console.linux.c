
#include "console.h"

#if defined(PLATFORM_LINUX)

UINT32 WriteConsole(const CHAR *output, USIZE outputLength)
{
#if defined(PLATFORM_LINUX_X86_64)

    // write(int fd, const void *buf, size_t count);
    // rax = 1 (SYS_write), rdi = fd, rsi = buf, rdx = count
    register INT64 rax asm("rax") = 1;   // SYS_write
    register INT64 rdi asm("rdi") = 1;   // STDOUT_FILENO
    register const CHAR *rsi asm("rsi") = output;
    register INT64 rdx asm("rdx") = (INT64)outputLength;

    __asm__ volatile(
        "syscall"
        : "+r"(rax)
        : "r"(rdi), "r"(rsi), "r"(rdx)
        : "rcx", "r11", "memory", "cc");

    //ret = rax;

#elif defined(PLATFORM_LINUX_AARCH64)

    // write: x8 = 64 (SYS_write), x0 = fd, x1 = buf, x2 = len
    register INT64 x8 asm("x8") = 64;    // SYS_write on AArch64
    register INT64 x0 asm("x0") = 1;     // STDOUT_FILENO
    register const CHAR *x1 asm("x1") = output;
    register INT64 x2 asm("x2") = (INT64)outputLength;

    __asm__ volatile(
        "svc 0"
        : "+r"(x0)
        : "r"(x1), "r"(x2), "r"(x8)
        : "memory", "cc");

    //ret = x0;

#elif defined(PLATFORM_LINUX_I386)

    // write: eax=4, ebx=fd, ecx=buf, edx=len, int 0x80
    INT32 ret;
    __asm__ volatile(
        "movl $4, %%eax\n\t"   /* SYS_write */
        "movl $1, %%ebx\n\t"   /* STDOUT_FILENO */
        "movl %1, %%ecx\n\t"   /* buffer */
        "movl %2, %%edx\n\t"   /* length */
        "int $0x80\n\t"
        "movl %%eax, %0\n\t"   /* return value */
        : "=r"(ret)
        : "r"(output), "r"((INT32)outputLength)
        : "eax", "ebx", "ecx", "edx", "memory", "cc");

#else
# error Unsupported platform
#endif

    // You probably want to return what kernel returned, but if you really
    // want "pretend everything was written", keep outputLength.
    return (UINT32)outputLength;
}

#endif