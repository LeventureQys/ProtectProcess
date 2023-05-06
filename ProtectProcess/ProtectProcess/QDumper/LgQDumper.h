#pragma once

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
class LgQDumper
{
public:
	LgQDumper();
};
//注意下面2个头文件引入的顺序


extern char* g_qsDumpName;

class QtDumpGenerate
{
public:
	QtDumpGenerate();

	void ApplicationCrashHandler(EXCEPTION_POINTERS* pException);
};

// dump处理
// 使用方法： SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
//编写dump文件，检查报错 
//QString g_qsDumpName = "LgInteractSmartCompus";
//SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
long ApplicationCrashHandler(EXCEPTION_POINTERS* pException);
#endif // _WIN32


