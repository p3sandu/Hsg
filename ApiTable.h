#pragma once
#include "FDefs.h"
#include "Hashes.h"
#include <windows.h>

#define LIST_NTDLL \
    X(HASH_FN_NtAllocateVirtualMemory, NtAllocateVirtualMemory, fnNtAllocateVirtualMemory) \
    X(HASH_FN_NtProtectVirtualMemory,  NtProtectVirtualMemory,  fnNtProtectVirtualMemory)  \
    X(HASH_FN_NtCreateThreadEx,        NtCreateThreadEx,        fnNtCreateThreadEx)

#define LIST_KERNEL32 \
    X(HASH_FN_GetTickCount,            GetTickCount,            fnGetTickCount)

#define LIST_KERNELBASE \
    X(HASH_FN_GetCurrentProcessId,     GetCurrentProcessId,     fnGetCurrentProcessId) \
    X(HASH_FN_WaitForSingleObject,     WaitForSingleObject,     fnWaitForSingleObject)

enum
{
#define X(fh, n, t) IDX_NT_##n,
    LIST_NTDLL
#undef X
        COUNT_NTDLL
};

enum
{
#define X(fh, n, t) IDX_K32_##n,
    LIST_KERNEL32
#undef X
        COUNT_KERNEL32
};

enum
{
#define X(fh, n, t) IDX_KB_##n,
    LIST_KERNELBASE
#undef X
        COUNT_KERNELBASE
};

typedef struct _API_ENTRY
{
    UINT32 u32Hash;
    PVOID lpAddr;
} API_ENTRY, *PAPI_ENTRY;

typedef struct _NTDLL_TABLE
{
    union
    {
        API_ENTRY Entries[COUNT_NTDLL];
        struct
        {
#define X(fh, n, t) API_ENTRY n;
            LIST_NTDLL
#undef X
        };
    };
} NTDLL_TABLE;

typedef struct _KERNEL32_TABLE
{
    union
    {
        API_ENTRY Entries[COUNT_KERNEL32];
        struct
        {
#define X(fh, n, t) API_ENTRY n;
            LIST_KERNEL32
#undef X
        };
    };
} KERNEL32_TABLE;

typedef struct _KERNELBASE_TABLE
{
    union
    {
        API_ENTRY Entries[COUNT_KERNELBASE];
        struct
        {
#define X(fh, n, t) API_ENTRY n;
            LIST_KERNELBASE
#undef X
        };
    };
} KERNELBASE_TABLE;

typedef struct _API_CONTEXT
{
    NTDLL_TABLE Nt;
    KERNEL32_TABLE K32;
    KERNELBASE_TABLE Kb;
} API_CONTEXT, *PAPI_CONTEXT;

extern API_CONTEXT API;

BOOL API_Initialize(void);
