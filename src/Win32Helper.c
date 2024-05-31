#include "Win32Helper.h"

HMODULE GetModuleByHash(DWORD64 hash)
{
    PLDR_MODULE     LDTEntry;
    PLIST_ENTRY     Head;
    PLIST_ENTRY     Current;
    PPEB            PEB;

#ifdef _M_IX86
    PEB = (PPEB)__readfsdword(0x30)
#else
    PEB = (PPEB)__readgsqword(0x60);
#endif
    Head = (PLIST_ENTRY)&PEB->LoaderData->InLoadOrderModuleList;
    Current = (PLIST_ENTRY)Head->Flink;

    do {
        CHAR ModuleName[256] = { 0 };


        LDTEntry = (PLDR_MODULE)Current;

        WCharStringToCharString(ModuleName, LDTEntry->BaseDllName.Buffer, LDTEntry->BaseDllName.Length);

        ToLowerStringA(ModuleName);

        if (ResolveHash(ModuleName) == hash || !hash)
        {
            return (HMODULE)LDTEntry->BaseAddress;
        }

        Current = (PLIST_ENTRY)Current->Flink;

    } while (Current != Head);

    return (NULL);
}

FARPROC GetProcAddressByHash(HMODULE hModule, DWORD64 hash)
{
    PBYTE                   ImageBase = (PBYTE)hModule;
    HMODULE                 fwdModule = NULL;
    FARPROC                 ProcAddress = NULL;
    PCHAR                   DotPtr = NULL;
    CHAR                    Library[MAX_PATH] = { 0 };
    CHAR                    Function[MAX_PATH] = { 0 };
    PIMAGE_DOS_HEADER       DosHdr = (PIMAGE_DOS_HEADER)ImageBase;
    PIMAGE_NT_HEADERS       NtHdr = (PIMAGE_NT_HEADERS)(ImageBase + DosHdr->e_lfanew);
    PIMAGE_OPTIONAL_HEADER  OptHdr = (PIMAGE_OPTIONAL_HEADER) & (NtHdr->OptionalHeader);
    PIMAGE_DATA_DIRECTORY   ExportDataDir = (PIMAGE_DATA_DIRECTORY) & (OptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
    PIMAGE_EXPORT_DIRECTORY ExportDir = (PIMAGE_EXPORT_DIRECTORY)(ImageBase + ExportDataDir->VirtualAddress);

    PDWORD pEAT = (PDWORD)(ImageBase + ExportDir->AddressOfFunctions);
    PDWORD pFunctionNameTable = (PDWORD)(ImageBase + ExportDir->AddressOfNames);
    PWORD  pOrdinals = (PWORD)(ImageBase + ExportDir->AddressOfNameOrdinals);

    for (DWORD i = 0; i < ExportDir->NumberOfNames; i++)
    {
        PSTR ExportName = (PSTR)(ImageBase + pFunctionNameTable[i]);

        if (ResolveHash(ExportName) == hash)
        {
            ProcAddress = (FARPROC)(ImageBase + pEAT[pOrdinals[i]]);
            break;
        }
    }

    if ((PBYTE)ProcAddress >= (PBYTE)ExportDir &&
        (PBYTE)ProcAddress < (PBYTE)(ExportDir + ExportDataDir->Size))
    {
        CHAR    fwdFunction[MAX_PATH] = { 0 };

        CopyMemoryEx(fwdFunction, (PCHAR)ProcAddress, StringLengthA((PCHAR)ProcAddress));

        DotPtr = StringLocateCharA((PCHAR)fwdFunction, '.');

        if (DotPtr)
            *DotPtr = 0;

        CopyMemoryEx(Library, fwdFunction, StringLengthA(fwdFunction));
        CopyMemoryEx(Function, DotPtr + 1, StringLengthA(DotPtr + 1));

        fwdModule = ModuleLoader(Library);

        if (fwdModule)
            ProcAddress = GetProcAddressByHash(fwdModule, ResolveHash(Function));
    }

    return (FARPROC)ProcAddress;
}

HMODULE ModuleLoader(LPSTR module)
{
    HANDLE          hModule = NULL;
    HMODULE         hNtdll = NULL;
    UNICODE_STRING  ModuleName = { 0 };
    WCHAR           ModuleNameW[MAX_PATH] = { 0 };

    hNtdll = GetModuleByHash(HASH_NTDLL);

    if (hNtdll)
    {
        RTLINITUNICODESTRING  RtlInitUnicodeString = (RTLINITUNICODESTRING)GetProcAddressByHash(hNtdll, HASH_RTLINITUNICODESTRING);
        LDRLOADDLL            LdrLoadDll = (LDRLOADDLL)GetProcAddressByHash(hNtdll, HASH_LDRLOADDLL);

        if (RtlInitUnicodeString && LdrLoadDll)
        {
            CharStringToWCharString(ModuleNameW, module, StringLengthA(module));

            RtlInitUnicodeString(&ModuleName, ModuleNameW);

            if (NT_SUCCESS(LdrLoadDll(NULL, 0, &ModuleName, &hModule)))
                return ((HMODULE)hModule);
            else
                return (NULL);
        }

    }

    return (NULL);

}

LPVOID ProcessHeap()
{
    PPEB            PEB;

#if defined(_WIN64)
    PEB = (PPEB)__readgsqword(0x60);
#elif define(_WIN32)
    PEB = (PPEB)__readfsdword(0x30);
#endif

    return (PEB->ProcessHeap);
}

DWORD64 ResolveHash(PSTR str)
{
    DWORD64 Hash = 0xBAADF00D;
    int     c;

    while (c = *str++)
        Hash = ((Hash << 0x5) + Hash) + c;

    return Hash;
}
