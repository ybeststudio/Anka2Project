#include "StdAfx.h"

#include <stdio.h>
#include <time.h>
#include <winsock.h>
#include <imagehlp.h>

FILE * fException;

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
    strftime(buffer, sizeof(buffer), "errorlog\\ErrorLog_%d-%m-%Y_%H-%M-%S.txt", timeinfo); // errorlog klasörünü oluştur

    FILE* fException = fopen(buffer, "wt");
    if (fException)
    {
        char module_name[256];
        time_t module_time;

        HMODULE hModule = GetModuleHandle(NULL);

        GetModuleFileName(hModule, module_name, sizeof(module_name));
        module_time = (time_t)GetTimestampForLoadedLibrary(hModule);

        fprintf(fException, "Module Name: %s\n", module_name);
        fprintf(fException, "Time Stamp: 0x%08x - %s\n", module_time, ctime(&module_time));
        fprintf(fException, "\n");
        fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
        fprintf(fException, "\n");

        CONTEXT& context = *pExceptionInfo->ContextRecord;

        fprintf(fException, "eax: 0x%08x\tebx: 0x%08x\n", context.Eax, context.Ebx);
        fprintf(fException, "ecx: 0x%08x\tedx: 0x%08x\n", context.Ecx, context.Edx);
        fprintf(fException, "esi: 0x%08x\tedi: 0x%08x\n", context.Esi, context.Edi);
        fprintf(fException, "ebp: 0x%08x\tesp: 0x%08x\n", context.Ebp, context.Esp);
        fprintf(fException, "\n");

        STACKFRAME stackFrame = { 0, };
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
		fprintf(fException, "Time Stamp: 0x%08x - %s\n", static_cast<unsigned char>(module_time), ctime(&module_time));
		fprintf(fException, "\n");
		fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
		fprintf(fException, "\n");
		
		CONTEXT&	context		= *pExceptionInfo->ContextRecord;

		fprintf(fException, "eax: 0x%08x\tebx: 0x%08x\n", context.Eax, context.Ebx);
		fprintf(fException, "ecx: 0x%08x\tedx: 0x%08x\n", context.Ecx, context.Edx);
		fprintf(fException, "esi: 0x%08x\tedi: 0x%08x\n", context.Esi, context.Edi);
		fprintf(fException, "ebp: 0x%08x\tesp: 0x%08x\n", context.Ebp, context.Esp);
		fprintf(fException, "\n");

		STACKFRAME stackFrame = {0,};
		stackFrame.AddrPC.Offset	= context.Eip;
		stackFrame.AddrPC.Mode		= AddrModeFlat;
		stackFrame.AddrStack.Offset	= context.Esp;
		stackFrame.AddrStack.Mode	= AddrModeFlat;
		stackFrame.AddrFrame.Offset	= context.Ebp;
		stackFrame.AddrFrame.Mode	= AddrModeFlat;

		for (int i=0; i < 512 && stackFrame.AddrPC.Offset; ++i)
		{
			if (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stackFrame, &context, nullptr, nullptr, nullptr, nullptr) != FALSE)
			{
				fprintf(fException, "0x%08x\t", stackFrame.AddrPC.Offset);
				EnumerateLoadedModules(hProcess, (PENUMLOADED_MODULES_CALLBACK) EnumerateLoadedModulesProc, &stackFrame.AddrPC.Offset);
				fprintf(fException, "\n");
			}
			else
			{
				break;
			}
		}

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