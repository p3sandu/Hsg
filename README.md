* ApiTable.h - essential definitions
* Resolver.c - essential functions including iat/eat parsing logic
* Hasher.py - hashing logic to generate hashes.h
* Hashes.h - generated hashes
* FDefs.h - win32 api prototypes

## walkthrough

let's say you want to add WaitForSingleObject() from kernelbase.dll.

### 1. ApiTable.h

define the macro list for the module:

```c
#define LIST_KERNELBASE \
    X(HASH_FN_GetCurrentProcessId,      GetCurrentProcessId,     fnGetCurrentProcessId) \
    X(HASH_FN_WaitForSingleObject,      WaitForSingleObject,     fnWaitForSingleObject)

```

add the index enum:

```c
enum
{
#define X(fh, n, t) IDX_KB_##n,
    LIST_KERNELBASE
#undef X
    COUNT_KERNELBASE
};

```

define the table structure:

```c
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

```

add the structure to the main API_CONTEXT:

```c
typedef struct _API_CONTEXT
{
    NTDLL_TABLE Nt;
    KERNEL32_TABLE K32;
    KERNELBASE_TABLE Kb;
} API_CONTEXT, *PAPI_CONTEXT;

```

### 2. Hasher.py

add lines to generate hashes for the module and api:

```python
f.write(f"#define HASH_MOD_KERNELBASE 0x{djb2_hash('kernelbase.dll', is_wide=True):08x}\n")
f.write(f"#define HASH_FN_WaitForSingleObject 0x{djb2_hash('WaitForSingleObject'):08x}\n")
```

run the script to update Hashes.h.

### 3. Resolver.c

add resolving logic inside API_Initialize():

```c
HMODULE hKb = GetModuleBaseByHash(HASH_MOD_KERNELBASE);
if (!PopulateTable(hKb, API.Kb.Entries, COUNT_KERNELBASE))
    return FALSE;

```

### 4. FDefs.h & main

add the function prototype to FDefs.h:

```c
typedef DWORD (WINAPI* fnWaitForSingleObject)(HANDLE hHandle, DWORD dwMilliseconds);

```

finally you can call it like this:

```c
int main()
{
    if (!API_Initialize())
        return -1;

    fnWaitForSingleObject pWaitForSingleObject = (fnWaitForSingleObject)API.Kb.WaitForSingleObject.lpAddr;
    
    printf("[+] resolved WaitForSingleObject: 0x%p\n", pWaitForSingleObject);
    getchar();

    return 0;
}
```
