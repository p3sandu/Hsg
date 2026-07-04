#include "ApiTable.h"
#include "Structs.h"

#define X(fh, n, t) {fh, NULL},

API_CONTEXT API = {.Nt.Entries = {LIST_NTDLL},
#undef X

#define X(fh, n, t) {fh, NULL},
                   .K32.Entries = {LIST_KERNEL32},
#undef X

#define X(fh, n, t) {fh, NULL},
                   .Kb.Entries = {LIST_KERNELBASE}};
#undef X

static inline UINT32 HashA(LPCSTR s)
{
    UINT32 h = 5381;
    while (*s)
        h = ((h << 5) + h) + *s++;
    return h;
}

static inline UINT32 HashW(LPCWSTR s)
{
    UINT32 h = 5381;
    while (*s)
    {
        WCHAR c = *s;
        if (c >= L'A' && c <= L'Z')
            c += 32;
        h = ((h << 5) + h) + c;
        s++;
    }
    return h;
}

static PVOID FindAddressByHash(HMODULE hMod, UINT32 u32FuncHash)
{
    if (!hMod)
        return NULL;

    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)hMod;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((BYTE *)hMod + pDos->e_lfanew);
    DWORD dwExportRVA = pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    DWORD dwExportSize = pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

    if (dwExportRVA == 0)
        return NULL;

    PIMAGE_EXPORT_DIRECTORY pExp = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)hMod + dwExportRVA);
    DWORD *pdwNames = (DWORD *)((BYTE *)hMod + pExp->AddressOfNames);
    DWORD *pdwFuncs = (DWORD *)((BYTE *)hMod + pExp->AddressOfFunctions);
    WORD *pwOrds = (WORD *)((BYTE *)hMod + pExp->AddressOfNameOrdinals);

    for (DWORD i = 0; i < pExp->NumberOfNames; i++)
    {
        LPCSTR cszName = (LPCSTR)((BYTE *)hMod + pdwNames[i]);
        if (HashA(cszName) == u32FuncHash)
        {
            PVOID lpAddr = (PVOID)((BYTE *)hMod + pdwFuncs[pwOrds[i]]);

            if ((BYTE *)lpAddr >= (BYTE *)pExp && (BYTE *)lpAddr < ((BYTE *)pExp + dwExportSize))
            {
                // TODO
                return NULL;
            }
            return lpAddr;
        }
    }
    return NULL;
}

static HMODULE GetModuleBaseByHash(UINT32 u32ModHash)
{
#ifdef _WIN64
    PPEB pPeb = (PPEB)__readgsqword(0x60);
#else
    PPEB pPeb = (PPEB)__readfsdword(0x30);
#endif
    PLDR_DATA_TABLE_ENTRY pEntry = (PLDR_DATA_TABLE_ENTRY)pPeb->Ldr->InLoadOrderModuleList.Flink;
    while (pEntry->DllBase != NULL)
    {
        if (HashW(pEntry->BaseDllName.Buffer) == u32ModHash)
            return (HMODULE)pEntry->DllBase;
        pEntry = (PLDR_DATA_TABLE_ENTRY)pEntry->InLoadOrderLinks.Flink;
    }
    return NULL;
}

static BOOL PopulateTable(HMODULE hMod, PAPI_ENTRY pEntries, SIZE_T uCount)
{
    for (SIZE_T i = 0; i < uCount; i++)
    {
        pEntries[i].lpAddr = FindAddressByHash(hMod, pEntries[i].u32Hash);
        if (!pEntries[i].lpAddr)
            return FALSE;
    }
    return TRUE;
}

BOOL API_Initialize(void)
{
    HMODULE hNtdll = GetModuleBaseByHash(HASH_MOD_NTDLL);
    HMODULE hK32 = GetModuleBaseByHash(HASH_MOD_KERNEL32);
    HMODULE hKb = GetModuleBaseByHash(HASH_MOD_KERNELBASE);

    if (!PopulateTable(hNtdll, API.Nt.Entries, COUNT_NTDLL))
        return FALSE;

    if (!PopulateTable(hK32, API.K32.Entries, COUNT_KERNEL32))
        return FALSE;

    if (!PopulateTable(hKb, API.Kb.Entries, COUNT_KERNELBASE))
        return FALSE;

    return TRUE;
}
