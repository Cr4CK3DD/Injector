#include "Core.h"

DWORD64     dwSSN           = 0x00;
LPVOID      pSyscallAddr    = NULL;

VOID InitilizeSysFunc( LPVOID pSyscallStub )
{
    dwSSN           = ((PBYTE)(pSyscallStub + 4))[0];
    pSyscallAddr    = pSyscallStub + 0x12;
}

NTSTATUS IndirectSyscall()
{
    __asm__(
        "xor r13, r13\n"
        "mov r13, rax\n"
        :
        :"r" (dwSSN)
    );

    __asm__(
        "xor r12, r12\n"
        "mov r12, rax\n"
        :
        :"r" (pSyscallAddr)
    );

    dwSSN           = 0x0;
    pSyscallAddr    = NULL;
    
    __asm__(
        "mov r10, rcx\n"
        "mov rax, r13\n"
        "jmp r12\n"
    );
}
