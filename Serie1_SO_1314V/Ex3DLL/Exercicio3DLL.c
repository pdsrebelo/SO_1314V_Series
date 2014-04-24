#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>
#include "Exercicio3DLL.h"

BOOL MEMINFOAPI GetSystemMemInfo(_Out_ PSYSMEMINFO pSysMemInfo){
	return FALSE;
}

BOOL MEMINFOAPI GetProcMemInfo(_In_ DWORD dwProcId, _Out_ PPROCMEMINFO pProcMemInfo){
	return FALSE;
}

VOID MEMINFOAPI PrintProcVirtualAddress(_In_ DWORD dwProcId){
	return NULL;
}