#include <Windows.h>

typedef NTSTATUS(NTAPI *fnNtAllocateVirtualMemory)(HANDLE, PVOID *, ULONG_PTR, PSIZE_T, ULONG, ULONG);
typedef NTSTATUS(NTAPI *fnNtProtectVirtualMemory)(HANDLE, PVOID *, PSIZE_T, ULONG, PULONG);
typedef LPVOID(WINAPI *fnVirtualAlloc)(LPVOID, SIZE_T, DWORD, DWORD);
typedef DWORD(WINAPI *fnGetTickCount)(VOID);
typedef DWORD(WINAPI *fnGetCurrentProcessId)(VOID);
typedef DWORD(WINAPI *fnWaitForSingleObject)(HANDLE hHandle, DWORD dwMilliseconds);
