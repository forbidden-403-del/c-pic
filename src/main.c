#include "environment.h"
#include "console.h"

ENTRYPOINT DOUBLE _start(VOID)
{
#if defined(PLATFORM_WINDOWS_I386)
	static ENVIRONMENT_DATA envData;
	GetCurrentPEB()->SubSystemData = (PVOID)&envData;

	PCHAR currentAddress = GetInstructionAddress();																	// Get the return address of the caller function
	UINT16 functionPrologue = 0x8955;																				// i386 function prologue: push ebp; mov ebp, esp
	PCHAR functionStart = ReversePatternSearch(currentAddress, (PCHAR)&functionPrologue, sizeof(functionPrologue)); // Scan backward for function prologue

	PPEB peb = GetCurrentPEB();
	PPEB_LDR_DATA ldr = peb->LoaderData;
	PLIST_ENTRY list = &ldr->InMemoryOrderModuleList;
	PLIST_ENTRY flink = list->Flink;
	PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
	USIZE EntryPoint = (USIZE)entry->EntryPoint;

	BOOL shouldRelocate = (EntryPoint != (USIZE)functionStart);
	SetEnvironmentBaseAddress(functionStart, shouldRelocate);

#endif

	for (DOUBLE i = MAKE_DOUBLE(0); i < MAKE_DOUBLE(100000); i += MAKE_DOUBLE(0.1))
	{
		PrintFormatedString(UTF8("Value: %.2f / %.2f\n"), i, MAKE_DOUBLE(100000));
	}

	ExitProcess(0);
}