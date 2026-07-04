ApiTable.h - contains the essential definitions
Resolver.c - contains the essential functions including iat/eat parsing logic
Hasher.py - contains the hashing logic, you can change the hashing function as your need. this generates hashes.h file with hashes
Hashes.h - contains generated hashes
FDefs.h - contains win32 api prototypes

## walkthrough
let's say you want to add WaitForSingleObject() from kernelbase.dll.
then you should first go to the ApiTable.h and make sure the module and api is defined like this,
```c
#define LIST_KERNELBASE \
    X(HASH_FN_GetCurrentProcessId,     GetCurrentProcessId,     fnGetCurrentProcessId) \
    X(HASH_FN_WaitForSingleObject,     WaitForSingleObject,     fnWaitForSingleObject)
```
then add the definition for enum for the newly added module like this,
```c
enum
{
#define X(fh, n, t) IDX_KB_##n,
    LIST_KERNELBASE
#undef X
        COUNT_KERNELBASE
};
```
then add the table definition for the newly added module
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
and add that structure to the main API_CONTEXT structure
```c
typedef struct _API_CONTEXT
{
    NTDLL_TABLE Nt;
    KERNEL32_TABLE K32;
    KERNELBASE_TABLE Kb;
} API_CONTEXT, *PAPI_CONTEXT;
```
then go to Hasher.py and add a new line to define the api for the new module and api like this,
```python
f.write(f"#define HASH_MOD_KERNELBASE 0x{djb2_hash('kernelbase.dll', is_wide=True):08x}\n")
f.write(f"#define HASH_FN_WaitForSingleObject 0x{djb2_hash('WaitForSingleObject'):08x}\n")
```
then run it and see whether the hashes are added if you don't trust me
after that go to resolver.c and add the resolving logic like this inside API_Initialize()
```c
HMODULE hKb = GetModuleBaseByHash(HASH_MOD_KERNELBASE);
if (!PopulateTable(hKb, API.Kb.Entries, COUNT_KERNELBASE))
	return FALSE;
```
then use it in main after defining the function prototype in FDefs.h
```c
int main() {
    if (!API_Initialize()) {
        return -1;
    }

    fnWaitForSingleObject pWaitForSingleObject = (fnWaitForSingleObject)API.Kb.WaitForSingleObject.lpAddr;
    
    printf("[+] resolved WaitForSingleObject: 0x%p\n", pWaitForSingleObject);
    getchar();

    return 0;
}
```
