#include "StringManipulation.h"

// https://github.com/vxunderground/VX-API/blob/main/VX-API/StringLength.cpp
SIZE_T StringLengthA(_In_ LPCSTR String)
{
    LPCSTR String2;

    for (String2 = String; *String2; ++String2);

    return (String2 - String);
}

// https://github.com/vxunderground/VX-API/blob/main/VX-API/StringLocateChar.cpp
PCHAR StringLocateCharA(_Inout_ PCHAR String, _In_ INT Character)
{
    do
    {
        if (*String == Character)
            return (PCHAR)String;

    } while (*String++);

    return NULL;
}

// https://github.com/vxunderground/VX-API/blob/main/VX-API/CharStringToWCharString.cpp
SIZE_T CharStringToWCharString(_Inout_ PWCHAR Destination, _In_ PCHAR Source, SIZE_T _In_ MaximumAllowed)
{
    INT Length = (INT)MaximumAllowed;

    while (--Length >= 0)
    {
        if (!(*Destination++ = *Source++))
            return MaximumAllowed - Length - 1;
    }

    return MaximumAllowed - Length;
}

PCHAR ToLowerStringA(_In_ PCHAR Ptr)
{
    PCHAR sv = Ptr;
    while (*sv != '\0')
    {
        if (*sv >= 'A' && *sv <= 'Z')
            *sv = *sv + ('a' - 'A');

        sv++;
    }
    return Ptr;
}

// https://github.com/vxunderground/VX-API/blob/main/VX-API/WCharStringToCharString.cpp
SIZE_T WCharStringToCharString(_Inout_ PCHAR Destination, _In_ PWCHAR Source, _In_ SIZE_T MaximumAllowed)
{
    INT Length = (INT)MaximumAllowed;

    while (--Length >= 0)
    {
        if (!(*Destination++ = *(PCHAR)Source++))
            return MaximumAllowed - Length - 1;
    }
    return MaximumAllowed - Length;
}

// https://github.com/vxunderground/VX-API/blob/main/VX-API/StringCompare.cpp
INT StringCompareA(_In_ LPCSTR String1, _In_ LPCSTR String2)
{
    for (; *String1 == *String2; String1++, String2++)
    {
        if (*String1 == '\0')
            return 0;
    }

    return ((*(LPCSTR)String1 < *(LPCSTR)String2) ? -1 : +1);
}

// https://github.com/vxunderground/VX-API/blob/main/VX-API/CopyMemoryEx.cpp
PVOID CopyMemoryEx(_Inout_ PVOID Destination, _In_ CONST PVOID Source, _In_ SIZE_T Length)
{
    PBYTE D = (PBYTE)Destination;
    PBYTE S = (PBYTE)Source;

    while (Length--)
        *D++ = *S++;

    return Destination;
}