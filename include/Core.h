#ifndef _CORE_
#define _CORE_

#include "Win32Helper.h"
#include "Injector.h"
#include <stdio.h>

typedef struct {
    DWORD   Pid; 
} PARAMS, *PPARAMS;

typedef struct {
	struct {
		NTOPENPROCESS			NtOpenProcess;
		NTALLOCATEVIRTUALMEMORY NtAllocateVirtualMemory;
		NTWRITEVIRTUALMEMORY	NtWriteVirtualMemory;
		NTPROTECTVIRTUALMEMORY	NtProtectVirtualMemory;
		NTCREATETHREADEX		NtCreateThreadEx;
		
		HMODULE					Handle;
	} Ntdll;

} API, *PAPI;

NTSTATUS IndirectSyscall();
VOID InitilizeSysFunc(LPVOID);

#endif