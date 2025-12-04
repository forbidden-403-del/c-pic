#ifndef __PEB_H__
#define __PEB_H__

#if defined(PLATFORM_WINDOWS)

#include "string.h"

// Macro to get the containing record from a field pointer
#define CONTAINING_RECORD(address, type, field) ((type *)((PCHAR)(address) - (USIZE)(&((type *)0)->field)))

// Unicode string structure
typedef struct _UNICODE_STRING
{
    UINT16 Length;
    UINT16 MaximumLength;
    PWCHAR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// List entry structure for linked lists
typedef struct _LIST_ENTRY
{
    struct _LIST_ENTRY *Flink;
    struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

// Loader data table entry structure
typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

// Loader module structure
typedef struct _LDR_MODULE
{
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID BaseAddress;
    PVOID EntryPoint;
    UINT32 SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    UINT32 Flags;
    INT16 LoadCount;
    INT16 TlsIndex;
    LIST_ENTRY HashTableEntry;
    UINT32 TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

// Process Environment Block Loader Data
typedef struct _PEB_LDR_DATA
{
    UINT32 Length;
    UINT32 Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

// Process parameters structure
typedef struct _RTL_USER_PROCESS_PARAMETERS
{
    UINT32 MaximumLength;
    UINT32 Length;
    UINT32 Flags;
    UINT32 DebugFlags;
    PVOID ConsoleHandle;
    UINT32 ConsoleFlags;
    PVOID StandardInput;
    PVOID StandardOutput;
    PVOID StandardError;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

// Process Environment Block
typedef struct _PEB
{
    UINT8 InheritedAddressSpace;
    UINT8 ReadImageFileExecOptions;
    UINT8 BeingDebugged;
    UINT8 Spare;
    PVOID Mutant;
    PVOID ImageBase;
    PPEB_LDR_DATA LoaderData;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    PVOID SubSystemData;
} PEB, *PPEB;

// Function to get the current process's PEB pointer
PPEB GetCurrentPEB(VOID);
// Function to resolve module handle by its name
PVOID GetModuleHandleFromPEB(PPEB peb, const WCHAR *moduleName);

#endif // PLATFORM_WINDOWS

#endif // __PEB_H__
