#include "SDK.h"
#include <time.h>
#include <cstring>

char g_logFile[255] = "/home/josh/log.txt";
char g_debugLogFile[255] = "/home/josh/debugLog.txt";

void Log::Debug(const char* fmt, ...) 
{
	va_list va_alist;
	char szLogbuf[4096];
	char szParameters[4066];
	char szTimestamp[30];
	struct tm *current_tm;
	time_t current_time = time(NULL);
	FILE* file;

	current_tm = localtime(&current_time);
	sprintf(szTimestamp, "[%02d:%02d:%02d]{DEBUG} %%s\n", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);

	va_start(va_alist, fmt);
	vsnprintf(szParameters, sizeof(szParameters), fmt, va_alist);
	va_end(va_alist);
	sprintf(szLogbuf, szTimestamp, szParameters);
	if (file = fopen(g_debugLogFile, "a"))
	{
		fprintf(file, "%s", szLogbuf);
		fclose(file);
	}
}

void Log::Msg(const char* fmt, ...) {
	
	va_list va_alist;
	char szLogbuf[4096];
	char szParameters[4066];
	char szTimestamp[30];
	struct tm *current_tm;
	time_t current_time = time(NULL);
	FILE* file;

	current_tm = localtime(&current_time);
	sprintf(szTimestamp, "[%02d:%02d:%02d]{MSG} %%s\n", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);

	va_start(va_alist, fmt);
	vsnprintf(szParameters, sizeof(szParameters), fmt, va_alist);
	va_end(va_alist);
	sprintf(szLogbuf, szTimestamp, szParameters);
	if (file = fopen(g_logFile, "a"))
	{
		fprintf(file, "%s", szLogbuf);
		fclose(file);
	}
	
}

void Log::Error(const char* fmt, ...) {
	
	va_list va_alist;
	char szLogbuf[4096];
	char szParameters[4066];
	char szTimestamp[30];
	struct tm *current_tm;
	time_t current_time = time(NULL);
	FILE* file;

	current_tm = localtime(&current_time);
	sprintf(szTimestamp, "[%02d:%02d:%02d]{ERROR} %%s\n", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);

	va_start(va_alist, fmt);
	vsnprintf(szParameters, sizeof(szParameters), fmt, va_alist);
	va_end(va_alist);
	sprintf(szLogbuf, szTimestamp, szParameters);
	if (file = fopen(g_debugLogFile, "a"))
	{
		fprintf(file, "%s", szLogbuf);
		fclose(file);
	}

	//MessageBoxA(NULL, szLogbuf, "ERROR", MB_ICONERROR);
	
}

void Log::Fatal(const char* fmt, ...) {
	va_list va_alist;
	char szLogbuf[4096];
	char szParameters[4066];
	char szTimestamp[30];
	struct tm *current_tm;
	time_t current_time = time(NULL);
	FILE* file;

	current_tm = localtime(&current_time);
	sprintf(szTimestamp, "[%02d:%02d:%02d]{FATAL} %%s\n", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);

	va_start(va_alist, fmt);
	vsnprintf(szParameters, sizeof(szParameters), fmt, va_alist);
	va_end(va_alist);
	sprintf(szLogbuf, szTimestamp, szParameters);
	if (file = fopen(g_debugLogFile, "a"))
	{
		fprintf(file, "%s", szLogbuf);
		fclose(file);
	}

	//MessageBoxA(NULL, szLogbuf, "FATAL ERROR", MB_ICONERROR);

	// TODO ADD ME BACK
	exit(1);
}
