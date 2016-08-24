#pragma once
#include "SDK.h"
#ifdef __DEBUG
#define LOGDEBUG( X, ... ) Log::Debug( X, __VA_ARGS__ )
#else
#define LOGDEBUG( X, ... )
#endif

#define XASSERT( x ) if(x == NULL) Log::Fatal("%s was NULL! (file: %s, line: %d)", #x, __FILE__, __LINE__)

class Log
{
public:
	static void Debug(const char* fmt, ...);
	static void Msg(const char* fmt, ...);
	static void Error(const char* fmt, ...);
	static void Fatal(const char* fmt, ...);
};
