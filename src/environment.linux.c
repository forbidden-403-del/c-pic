#include "environment.h"

#if defined(PLATFORM_LINUX)

NO_RETURN VOID ExitProcess(USIZE code)
{

#if defined(PLATFORM_LINUX_X86_64)

	__asm__ volatile(
		"mov $60, %%rax\n" // syscall: exit = 60
		"mov %0, %%rdi\n"  // exit code
		"syscall\n"
		:
		: "r"(code)
		: "rax", "rdi");

#elif defined(PLATFORM_LINUX_AARCH64)

	register long x0 __asm__("x0") = code; // exit code
	register long x8 __asm__("x8") = 93;   // SYS_exit on aarch64

	__asm__ volatile(
		"svc 0"
		:
		: "r"(x0), "r"(x8)
		: "memory", "cc");

#elif defined(PLATFORM_LINUX_I386)

	__asm__ volatile(
		"movl $1, %%eax\n" // SYS_exit
		"movl %0, %%ebx\n" // exit code
		"int $0x80\n"
		:
		: "r"(code)
		: "eax", "ebx");
#else
#error "Unsupported architecture for ExitProcess implementation on Linux!"
#endif
	while (1)
	{
	} // Prevents compiler warnings about no return
}
#endif