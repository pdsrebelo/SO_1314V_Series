#ifndef EX3_DLL_
#define EX3_DLL_

#include <Windows.h>
#include <Psapi.h>

#ifdef _WINDLL
#define MEMINFOAPI __declspec(dllexport)
#else
#define MEMINFOAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SysMemInfo{
	ULONGLONG PhysTotal;
	ULONGLONG PhysAvailable;
	ULONGLONG VirtualTotal;
	ULONGLONG VirtualAvailable;
} SYSMEMINFO, *PSYSMEMINFO;

typedef struct ProcMemInfo{
	ULONGLONG UllTotalVirtual;
	ULONGLONG UllAvailVirtual;
	SIZE_T WorkingSetSize;
} PROCMEMINFO, *PPROCMEMINFO;

// Retorna informação da utilização global de memória do sistema.
BOOL MEMINFOAPI GetSystemMemInfo(_Out_ PSYSMEMINFO pSysMemInfo);

// Retorna informação da utilização de memória do processo.
BOOL MEMINFOAPI GetProcMemInfo(_In_ DWORD dwProcId, _Out_ PPROCMEMINFO pProcMemInfo);

// Apresenta na consola a lista de VAD's do processo.
VOID MEMINFOAPI PrintProcVirtualAddress(_In_ DWORD dwProcId);

#ifdef __cplusplus
}
#endif

#endif	/*EX3_DLL_*/