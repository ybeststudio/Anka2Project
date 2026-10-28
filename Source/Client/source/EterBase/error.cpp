#include "StdAfx.h"

#include <stdio.h>
#include <time.h>
#include <winsock.h>
#include <imagehlp.h>

FILE * fException;

#if defined(_WIN64)
BOOL CALLBACK EnumerateLoadedModulesProc64(PCSTR ModuleName, DWORD64 ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
	DWORD64 offset = *((DWORD64*)UserContext);

	if (offset >= ModuleBase && offset <= ModuleBase + ModuleSize)
	{
		fprintf(fException, "%s", ModuleName);
		return FALSE;
	}
	else
		return TRUE;
}
#endif

#if _MSC_VER >= 1400
BOOL CALLBACK EnumerateLoadedModulesProc(PCSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
#else
BOOL CALLBACK EnumerateLoadedModulesProc(PSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
#endif
{
	DWORD offset = *((DWORD*)UserContext);

	if (offset >= ModuleBase && offset <= ModuleBase + ModuleSize)
	{
		fprintf(fException, "%s", ModuleName);
		//__idx += sprintf(__msg+__idx, "%s", ModuleName);
		return FALSE;
	}
	else
		return TRUE;
}

static void DumpExceptionContext(HANDLE hProcess, HANDLE hThread, CONTEXT& context)
{
#if defined(_WIN64)
	fprintf(fException, "rax: 0x%016llx\trbx: 0x%016llx\n", context.Rax, context.Rbx);
	fprintf(fException, "rcx: 0x%016llx\trdx: 0x%016llx\n", context.Rcx, context.Rdx);
	fprintf(fException, "rsi: 0x%016llx\trdi: 0x%016llx\n", context.Rsi, context.Rdi);
	fprintf(fException, "rbp: 0x%016llx\trsp: 0x%016llx\n", context.Rbp, context.Rsp);
	fprintf(fException, "r8 : 0x%016llx\tr9 : 0x%016llx\n", context.R8,  context.R9);
	fprintf(fException, "r10: 0x%016llx\tr11: 0x%016llx\n", context.R10, context.R11);
	fprintf(fException, "r12: 0x%016llx\tr13: 0x%016llx\n", context.R12, context.R13);
	fprintf(fException, "r14: 0x%016llx\tr15: 0x%016llx\n", context.R14, context.R15);
	fprintf(fException, "rip: 0x%016llx\n", context.Rip);
	fprintf(fException, "\n");

	STACKFRAME64 stackFrame = { 0 };
	stackFrame.AddrPC.Offset = context.Rip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = context.Rsp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = context.Rbp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;

	for (int i = 0; i < 512 && stackFrame.AddrPC.Offset; ++i)
	{
		if (StackWalk64(IMAGE_FILE_MACHINE_AMD64, hProcess, hThread, &stackFrame, &context, nullptr, nullptr, nullptr, nullptr) != FALSE)
		{
			fprintf(fException, "0x%016llx\t", stackFrame.AddrPC.Offset);
			EnumerateLoadedModules64(hProcess, (PENUMLOADED_MODULES_CALLBACK64)EnumerateLoadedModulesProc64, &stackFrame.AddrPC.Offset);
			fprintf(fException, "\n");
		}
		else
		{
			break;
		}
	}
#else
	fprintf(fException, "eax: 0x%08x\tebx: 0x%08x\n", context.Eax, context.Ebx);
	fprintf(fException, "ecx: 0x%08x\tedx: 0x%08x\n", context.Ecx, context.Edx);
	fprintf(fException, "esi: 0x%08x\tedi: 0x%08x\n", context.Esi, context.Edi);
	fprintf(fException, "ebp: 0x%08x\tesp: 0x%08x\n", context.Ebp, context.Esp);
	fprintf(fException, "\n");

	STACKFRAME stackFrame = { 0 };
	stackFrame.AddrPC.Offset = context.Eip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = context.Esp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = context.Ebp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;

	for (int i = 0; i < 512 && stackFrame.AddrPC.Offset; ++i)
	{
		if (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL) != FALSE)
		{
			fprintf(fException, "0x%08x\t", stackFrame.AddrPC.Offset);
			EnumerateLoadedModules(hProcess, (PENUMLOADED_MODULES_CALLBACK)EnumerateLoadedModulesProc, &stackFrame.AddrPC.Offset);
			fprintf(fException, "\n");
		}
		else
		{
			break;
		}
	}
#endif
}

#ifdef ENABLE_TIMESTAMPED_EXCEPTION_DUMP
LONG __stdcall EterExceptionFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "errorlog\\ErrorLog_%d-%m-%Y_%H-%M-%S.txt", timeinfo);

    fException = fopen(buffer, "wt");
    if (fException)
    {
        char module_name[256];
        time_t module_time;

        HMODULE hModule = GetModuleHandle(NULL);

        GetModuleFileName(hModule, module_name, sizeof(module_name));
        module_time = (time_t)GetTimestampForLoadedLibrary(hModule);

        fprintf(fException, "Module Name: %s\n", module_name);
        fprintf(fException, "Time Stamp: 0x%08x - %s\n", (unsigned int)module_time, ctime(&module_time));
        fprintf(fException, "\n");
        fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
        fprintf(fException, "\n");

        DumpExceptionContext(hProcess, hThread, *pExceptionInfo->ContextRecord);

        fprintf(fException, "\n");
        fflush(fException);

        fclose(fException);
        fException = NULL;

        WinExec("errorlog.exe", SW_SHOW);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}
#else
LONG __stdcall EterExceptionFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
	HANDLE		hProcess	= GetCurrentProcess();
	HANDLE		hThread		= GetCurrentThread();

	fException = fopen("ErrorLog.txt", "wt");
	if (fException)
	{
		char module_name[256];
		time_t module_time;

		HMODULE hModule = GetModuleHandle(nullptr);

		GetModuleFileName(hModule, module_name, sizeof(module_name));
		module_time = (time_t)GetTimestampForLoadedLibrary(hModule);

		fprintf(fException, "Module Name: %s\n", module_name);
		fprintf(fException, "Time Stamp: 0x%08x - %s\n", static_cast<unsigned int>(module_time), ctime(&module_time));
		fprintf(fException, "\n");
		fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
		fprintf(fException, "\n");
		
		DumpExceptionContext(hProcess, hThread, *pExceptionInfo->ContextRecord);

		fprintf(fException, "\n");

		fflush(fException);

		fclose(fException);
		fException = nullptr;

		WinExec("errorlog.exe",SW_SHOW);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void SetEterExceptionHandler()
{
	SetUnhandledExceptionFilter(EterExceptionFilter);
}