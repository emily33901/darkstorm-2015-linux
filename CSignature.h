#pragma once

#include "SDK.h"

class CSignature
{
public:
	uintptr_t dwFindPattern(uintptr_t dwAddress, uintptr_t dwLength, const char* szPattern);
	void  *GetModuleHandleSafe( const char* pszModuleName );
 	uintptr_t GetClientSignature ( char* chPattern );
	uintptr_t GetEngineSignature ( char* chPattern );
};

extern CSignature gSignatures;
