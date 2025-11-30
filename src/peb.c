#include "peb.h"

#if defined(PLATFORM_WINDOWS)

// Returns the current process's PEB pointer
PPEB GetCurrentPEB(VOID) {
    PPEB peb;
#if defined(PLATFORM_WINDOWS_AMD64)

    __asm__("movq %%gs:%1, %0" : "=r" (peb) : "m" (*(PUINT64)(0x60)));

#elif defined(PLATFORM_WINDOWS_I386)

    __asm__("movl %%fs:%1, %0" : "=r" (peb) : "m" (*(PUINT32)(0x30)));

#elif defined(PLATFORM_WINDOWS_ARM32)

    __asm__("ldr %0, [r9, %1]" : "=r"(peb) : "i"(0x30));

#elif defined(PLATFORM_WINDOWS_ARM64)

	__asm__("ldr %0, [x18, #%1]" : "=r"(peb) : "i"(0x60));

#else
	#error Unsupported platform
#endif
    return peb;
}

// Get the base address of a module by its name
PVOID GetModuleHandleFromPEB(PPEB peb, const PWCHAR moduleName)
{
    PLIST_ENTRY list  = &peb->LoaderData->InMemoryOrderModuleList;
    PLIST_ENTRY entry = list->Flink;

    while (entry != list) 
    {
        PLDR_MODULE module = CONTAINING_RECORD(entry, LDR_MODULE, InMemoryOrderModuleList);

        if (module->BaseDllName.Buffer != NULL)
        {
            if (CompareWideStringIgnoreCase(module->BaseDllName.Buffer, moduleName))
            {
                return module->BaseAddress;
            }
        }

        entry = entry->Flink;
    }

    return NULL;
}

#endif // PLATFORM_WINDOWS