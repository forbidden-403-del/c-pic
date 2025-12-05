#include "environment.h"

#if defined(PLATFORM_WINDOWS_I386)

PCHAR GetInstructionAddress(VOID)
{
    return __builtin_return_address(0);
}

VOID InitEnvironmentData(PENVIRONMENT_DATA envData, PVOID baseAddress)
{
    PPEB peb = GetCurrentPEB();
	peb->SubSystemData = (PVOID)envData;

	PPEB_LDR_DATA ldr = peb->LoaderData;
	PLIST_ENTRY list = &ldr->InMemoryOrderModuleList;
	PLIST_ENTRY flink = list->Flink;
	PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
	USIZE EntryPoint = (USIZE)entry->EntryPoint;

	BOOL shouldRelocate = (EntryPoint != (USIZE)baseAddress);
	envData->BaseAddress = baseAddress;
	envData->ShouldRelocate = shouldRelocate;
}

PCHAR ReversePatternSearch(PCHAR rip, const CHAR *pattern, UINT32 len)
{
    PCHAR p = rip;
    while (1)
    {
        UINT32 i = 0;
        for (; i < len; i++)
        {
            if (p[i] != pattern[i])
                break;
        }
        if (i == len)
            return p; // found match
        p--;          // move backward
    }
}

// Logic:
// We support two execution modes:
//   1) Normal Windows executable:
//        - Image is loaded by the OS loader.
//        - Relocations are applied.
//        - All pointers to our image are based on ENV_BASE.
//   2) PIC blob:
//        - Image is mapped and executed manually.
//        - Relocations are NOT applied.
//        - All compiler-generated addresses still assume IMAGE_LINK_BASE.
//
// Runtime information:
//   - IMAGE_LINK_BASE: original image base used during linking (always 0x401000).
//   - ENV_BASE      : actual base address where the image is loaded in memory at runtime.
//   - pointer       : some absolute address that may be:
//                       * already relocated by the loader (ENV_BASE + offset), or
//                       * still at the original link-time location (IMAGE_LINK_BASE + offset), or
//                       * not part of our image at all.
//

PVOID RebaseLiteral(PVOID p)
{
    // Get Environment Data
    PENVIRONMENT_DATA EnvData = GetEnvironmentData();

    if (EnvData->ShouldRelocate)
    {
        // Need to relocate because we're running as PIC blob
        return (PVOID)((USIZE)p + (USIZE)EnvData->BaseAddress - IMAGE_LINK_BASE);
    }
    else
    {
        // Pointer is already relocated
        return p;
    }
}

#endif // PLATFORM_WINDOWS_I386