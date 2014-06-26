#include <windows.h>
#include <tchar.h>
#include <Psapi.h>
#include <stdio.h>
#include "Exercicio5_MEMINFO_DLL_C.h"

VOID printBlockAccessType(DWORD protection){
	char*allocationProtect;
	switch (protection) {
	case PAGE_EXECUTE:
		allocationProtect = "PAGE_EXECUTE"; break;
	case PAGE_EXECUTE_READ:
		allocationProtect = "PAGE_EXECUTE_READ"; break;
	case PAGE_EXECUTE_READWRITE:
		allocationProtect = "PAGE_EXECUTE_READWRITE"; break;
	case PAGE_EXECUTE_WRITECOPY:
		allocationProtect = "PAGE_EXECUTE_WRITECOPY"; break;
	case PAGE_GUARD:
		allocationProtect = "PAGE_GUARD"; break;
	case PAGE_NOACCESS:
		allocationProtect = "PAGE_NOACCESS"; break;
	case PAGE_NOCACHE:
		allocationProtect = "PAGE_NOCACHE"; break;
	case PAGE_READONLY:
		allocationProtect = "PAGE_READONLY"; break;
	case PAGE_READWRITE:
		allocationProtect = "PAGE_READWRITE"; break;
	case PAGE_WRITECOPY:
		allocationProtect = "PAGE_WRITECOPY"; break;
	default:
		allocationProtect = "?"; break;
	}
	printf("%s", allocationProtect);
}

VOID printBlockMemoryState(DWORD state){
	char* str;
	switch (state){
	case MEM_COMMIT:
		str = "MEM_COMMIT"; break;
	case MEM_FREE:
		str = "MEM_FREE"; break;
	case MEM_RESERVE:
		str = "MEM_RESERVE"; break;
	default:
		str = "?"; break;
	}
	printf("%s", str);
}

VOID printRegionType(DWORD type){
	char* str;
	switch (type){
	case MEM_IMAGE:
		str = "MEM_IMAGE"; break;
	case MEM_MAPPED:
		str = "MEM_MAPPED"; break;
	case MEM_PRIVATE:
		str = "MEM_PRIVATE"; break;
	default:
		str = ""; break;
	}
	printf("%s", str);
}

VOID printRegionInfo(DWORD nRegion, MEMORY_BASIC_INFORMATION memInfo, HANDLE hProc){

	printf("\n\nREGION # %i\n", nRegion);

	DWORD enderecoInicio = memInfo.BaseAddress;
	DWORD enderecoFim = enderecoInicio + memInfo.RegionSize;
	DWORD accessType = memInfo.AllocationProtect;

	printf("\nEndereco de inicio = %x ", enderecoInicio);
	printf("\nEndereco de fim = %x ", enderecoFim);
	printf("\nTipo de Acesso = "); printBlockAccessType(accessType);
	printf("\nProteccao com que a regiao foi alocada inicialmente = "); printBlockAccessType(memInfo.Protect);
	printf("\nEstado das paginas dentro da regiao = "); printBlockMemoryState(memInfo.State);
	printf("\nTipo das paginas dentro da regiao = "); printRegionType(memInfo.Type);

	// no caso do tipo de páginas  ser image,  indicar o ficheiro com o caminho absoluto no sistema de ficheiros de onde teve origem.
	if (memInfo.Type == MEM_IMAGE){
		// Procurar o Module que tem um endereço válido (entre o endereço de início e endereço de fim)!
		HMODULE modules[KiloB];
		DWORD cbNeeded;
		unsigned int i;
		if (EnumProcessModules(hProc, modules, sizeof(modules), &cbNeeded)){
			for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++){
				CHAR fileNameBuffer[MAX_PATH];

				if (modules[i] >= enderecoInicio && modules[i] <= enderecoFim)
				{
					// Obter o nome do caminho para o ficheiro
					if (GetModuleFileName(modules[i], fileNameBuffer, sizeof(fileNameBuffer)))
						//if (GetModuleFileNameEx(hProc, hMods[i], fileName, sizeof(szModName) / sizeof(TCHAR)))
					{
						printf("\nCaminho do FICHEIRO de origem = %s (0x%08X)\n", fileNameBuffer, modules[i]);
					}
				}
			}
		}
	}
}

BOOL MEMINFO_API C_GetSystemMemInfo(_Out_ PSYSMEMINFO_C pSysMemInfo){
	PPERFORMACE_INFORMATION ppinfo = malloc(sizeof(PERFORMACE_INFORMATION));
	ULONGLONG totalVirtualMem, totalVirtualMemAvail, totalPhysMem, totalPhysMemAvail;

	printf("~~~~ INFORMACAO GLOBAL ~~~~~");
	if (GetPerformanceInfo(ppinfo, sizeof(PERFORMACE_INFORMATION)) == FALSE)
		return FALSE;

	totalVirtualMem = ppinfo->PageSize * (ULONGLONG)ppinfo->CommitLimit;
	totalVirtualMemAvail = ppinfo->PageSize * (ULONGLONG)ppinfo->CommitTotal;
	totalPhysMem = ppinfo->PageSize*ppinfo->PhysicalTotal;
	totalPhysMemAvail = ppinfo->PageSize*ppinfo->PhysicalAvailable;

	printf("\nTotal de memoria fisica existente: %llu KiB = %llu MiB = %.2f GiB", totalPhysMem / KiloB, totalPhysMem / MegaB, totalPhysMem / GigaB);
	printf("\nTotal de memoria fisica disponivel: %llu KiB = %llu MiB = %.2f GiB", totalPhysMemAvail / KiloB, totalPhysMemAvail / MegaB, totalPhysMemAvail / GigaB);
	printf("\nTotal de memoria virtual existente (fisica + page file): %llu KiB = %llu MiB = %.2f GiB", totalVirtualMem / KiloB, totalVirtualMem / MegaB, totalVirtualMem / GigaB);
	printf("\nTotal de memoria virtual disponivel: %llu KiB = %llu MiB = %.2f GiB", totalVirtualMemAvail / KiloB, totalVirtualMemAvail / MegaB, totalVirtualMemAvail / GigaB);
	printf("\nClique em qualquer tecla para ver a informação local do processo..."); getchar();
	return TRUE;
}

VOID MEMINFO_API C_PrintProcVirtualAddress(_In_ DWORD dwProcId){
	HANDLE hProc = NULL;
	MEMORY_BASIC_INFORMATION memInfo;
	CHAR * lpAddress = 0;
	DWORD nRegion = 1;

	if (hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, dwProcId) == NULL){
		printf("ERROR: %s\n", GetLastError());
		return FALSE;
	}

	while (VirtualQueryEx(hProc, lpAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)) != 0){
		lpAddress = (char*)memInfo.BaseAddress + memInfo.RegionSize;
		if (memInfo.State == MEM_FREE)
			continue;
		printRegionInfo(nRegion, memInfo, hProc);
		nRegion++;
	}
}

BOOL MEMINFO_API C_GetProcMemInfo(_In_  DWORD  dwProcId, _Out_  PPROCMEMINFO_C pProcMemInfo){
	HANDLE hProc = NULL;
	MEMORYSTATUSEX status;
	PROCESS_MEMORY_COUNTERS procMemCtr;

	printf("\n\n~~~~ INFORMACAO LOCAL DO PROCESSO ~~~~~");

	status.dwLength = sizeof (status);

	if (hProc = OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, dwProcId) == NULL || !GlobalMemoryStatusEx(&status) ||
		!GetProcessMemoryInfo(hProc, &procMemCtr, sizeof(PROCESS_MEMORY_COUNTERS))){
		printf("ERROR: %s\n", GetLastError());
		return FALSE;
	}

	printf("\nProcess ID = %u", GetProcessId(hProc));
	printf("\nTotal de espaco de enderecamento virtual existente: %llu KiB = %llu MiB", status.ullTotalVirtual / KiloB, status.ullTotalVirtual / MegaB);
	printf("\nTotal de espaco de enderecamento virtual disponivel: %llu KiB = %llu MiB", status.ullAvailVirtual / KiloB, status.ullAvailVirtual / MegaB);
	printf("\nDimensao do Working Set: %u KiB = %.2f MiB", procMemCtr.WorkingSetSize / KiloB, (double)procMemCtr.WorkingSetSize / MegaB);

	// Afectar a struct _out_
	pProcMemInfo->processId = dwProcId;
	pProcMemInfo->workingSetSize = procMemCtr.WorkingSetSize;
	pProcMemInfo->totalVirtualSpace = status.ullTotalVirtual;
	pProcMemInfo->availableVirtualSpace = status.ullAvailVirtual;

	printf("\n\nClique em qualquer tecla para terminar..."); getchar();
	CloseHandle(hProc);
	return TRUE;
}