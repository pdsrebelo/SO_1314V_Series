#include "stdafx.h"
#include <stdio.h>

#include "MemInfo_DLL.h"

using namespace std;

namespace MemInfoFuncs{

	BOOL MyMemInfoFuncs::GetSystemMemInfo(_Out_  PSYSMEMINFO  pSysMemInfo){  //  Retorna informação da utilização global de memória do sistema.
		printf("Get System Mem Info");
		return true;
	}

	VOID MyMemInfoFuncs::PrintProcVirtualAddress(_In_ DWORD dwProcId){ // Apresenta na consola a lista de VAD’s do processo.
		printf("Print Proc Virtual Address");

	}

	BOOL MyMemInfoFuncs::GetProcMemInfo(_In_  DWORD  dwProcId, _Out_  PPROCMEMINFO pProcMemInfo){ // Retorna informação da utilização de memória do processo.
		printf("Get Proc Mem Info");
		return true;
	}
}