#ifndef _STRING_MANIPULATION_
# define _STRING_MANIPULATION_

#include "Native.h"

INT		StringCompareA(LPCSTR, LPCSTR);
PVOID	CopyMemoryEx(PVOID, CONST PVOID, SIZE_T);
SIZE_T	WCharStringToCharString(PCHAR, PWCHAR, SIZE_T);
SIZE_T	CharStringToWCharString(PWCHAR, PCHAR, SIZE_T);
SIZE_T	StringLengthA(LPCSTR);
PCHAR	StringLocateCharA(PCHAR, INT);
PCHAR	ToLowerStringA(PCHAR);

#endif
