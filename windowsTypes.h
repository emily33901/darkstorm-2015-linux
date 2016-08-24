#pragma once

#include <dlfcn.h>
#include <string>
#include <algorithm>
#include <cstdint>

// redefinitions of some basic winapi functions that call the linux equivilants

/*
typedef uintptr_t DWORD;
typedef void *PVOID, *HMODULE;
typedef unsigned char BYTE, byte, *PBYTE;
typedef unsigned short WORD, *PWORD;
*/

// these dont exist outside of msvc
// __thiscall is just __cdecl with the thisptr first on the stack
// these macros will remove their effect

/*
#define __thiscall
#define __fastcall
#define __stdcall
#define __cdecl
*/

inline void *GetProcAddress (void *dlHandle, const char *sym)
{
	return dlsym(dlHandle, sym);
}
