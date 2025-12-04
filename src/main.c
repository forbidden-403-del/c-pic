#include "environment.h"
#include "console.h"

#define MAKE_DOUBLE(name, i)   \
	INT64 name##INT64 = i##LL; \
	DOUBLE name = (DOUBLE)name##INT64;

ENTRYPOINT DOUBLE _start(VOID)
{
#if defined(PLATFORM_WINDOWS_I386)

	PCHAR currentAddress = GetInstructionAddress(); // Get the return address of the caller function

	UINT16 functionPrologue = 0x8955; // i386 function prologue: push ebp; mov ebp, esp

	PCHAR functionStart = ReversePatternSearch(currentAddress, (PCHAR)&functionPrologue, sizeof(functionPrologue)); // Scan backward for function prologue

	SetEnvironmentBaseAddress(functionStart); // Set the environment base address to the start of the function
#endif

	MAKE_DOUBLE(one, 1);
	MAKE_DOUBLE(two, 2);

	PrintFormatedString(UTF8("Example DOUBLE: %f\n"), one / two);

	ExitProcess(0);
}