#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include "primitives.h"

#if defined(PLATFORM_WINDOWS)

#include "peb.h"
#include "pe.h"

#if defined(PLATFORM_WINDOWS_I386)

// -----------------------------------------------------------------------------
// Environment base management (used for PIC-style rebasing)
// -----------------------------------------------------------------------------
#define IMAGE_LINK_BASE ((USIZE)0x401000)

#define GetEnvironmentBaseAddress() (GetCurrentPEB()->SubSystemData)
#define SetEnvironmentBaseAddress(v) (GetCurrentPEB()->SubSystemData = (PVOID)(v))

// These macros are only meaningful when GetEnvironmentBaseAddress() is valid.
#define ENV_BASE ((USIZE)(GetEnvironmentBaseAddress()))
#define ENV_DELTA (ENV_BASE - IMAGE_LINK_BASE)

#define UTF8(literal) ((PCHAR)RebaseLiteral((PVOID)(literal)))
#define UTF16(literal) ((PWCHAR)RebaseLiteral((PVOID)(literal)))

// Get the caller's return address
PCHAR GetInstructionAddress(VOID);

// Scan backward in memory for a specific byte pattern
PCHAR ReversePatternSearch(PCHAR rip, const CHAR *pattern, UINT32 len);

PVOID RebaseLiteral(PVOID p);

#else // !PIC
#define UTF8(s) (s)
#define UTF16(s) (s)
#endif // PIC

// -----------------------------------------------------------------------------
// ExitProcess
// -----------------------------------------------------------------------------
// This is just a placeholder macro for the Windows platform.
// It is intended to be used ONLY in the entry point function.
// In any other function, using ExitProcess(code) will just translate to `return code`.
#define ExitProcess(code) return (code)

#elif defined(PLATFORM_LINUX)

NO_RETURN VOID ExitProcess(USIZE code);

#endif // PLATFORM_* checks

#endif // __ENVIRONMENT_H__
