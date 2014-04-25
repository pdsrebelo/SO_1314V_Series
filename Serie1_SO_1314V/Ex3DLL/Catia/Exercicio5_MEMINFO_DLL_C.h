#ifndef MEMINFO_DLL_H
#define MEMINFO_DLL_H

#include <windows.h>
#include <psapi.h>

#ifdef _WINDLL
#define MEMINFO_API __declspec(dllexport);
#else
#define MEMINFO_API __declspec(dllimport);
#endif

#define KiloB 1024
#define MegaB 1048576
#define GigaB 1073741824

typedef struct SysMemInfo{
	ULONGLONG totalVirtualMemory;
	ULONGLONG availableVirtualMemory;
	ULONGLONG totalPhysicalMemory;
	ULONGLONG availablePhysicalMemory;
}SYSMEMINFO_C, *PSYSMEMINFO_C;

typedef struct ProcMemInfo {
	DWORD processId;
	SIZE_T pageSize;
	SIZE_T workingSetSize;
	ULONGLONG totalVirtualSpace;
	ULONGLONG availableVirtualSpace;

} PROCMEMINFO_C, *PPROCMEMINFO_C;

VOID MEMINFO_API C_PrintProcVirtualAddress(_In_ DWORD dwProcId);
	
BOOL MEMINFO_API C_GetSystemMemInfo(_Out_ PSYSMEMINFO_C pSysMemInfo);

BOOL MEMINFO_API C_GetProcMemInfo(_In_  DWORD  dwProcId, _Out_  PPROCMEMINFO_C pProcMemInfo);

#endif
