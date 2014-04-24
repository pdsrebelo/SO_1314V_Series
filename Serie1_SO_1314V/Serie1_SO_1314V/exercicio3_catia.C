#include "windows.h"
#include "stdio.h"
#include "psapi.h"
#include "tchar.h"

#define MegaB 1048576
#define KiloB 1024
#define GB 1073741824

VOID printBlockAccessType(DWORD protection){
	char* allocationProtect = (char*)malloc(128);
	switch (protection) {
	case PAGE_EXECUTE:
		allocationProtect = "PAGE_EXECUTE";
		break;
	case PAGE_EXECUTE_READ:
		allocationProtect = "PAGE_EXECUTE_READ";
		break;
	case PAGE_EXECUTE_READWRITE:
		allocationProtect = "PAGE_EXECUTE_READWRITE";
		break;
	case PAGE_EXECUTE_WRITECOPY:
		allocationProtect = "PAGE_EXECUTE_WRITECOPY";
		break;
	case PAGE_GUARD:
		allocationProtect = "PAGE_GUARD";
		break;
	case PAGE_NOACCESS:
		allocationProtect = "PAGE_NOACCESS";
		break;
	case PAGE_NOCACHE:
		allocationProtect = "PAGE_NOCACHE";
		break;
	case PAGE_READONLY:
		allocationProtect = "PAGE_READONLY";
		break;
	case PAGE_READWRITE:
		allocationProtect = "PAGE_READWRITE";
		break;
	case PAGE_WRITECOPY:
		allocationProtect = "PAGE_WRITECOPY";
		break;
	default:
		allocationProtect = "?";
		break;
	}
	printf("%s", allocationProtect);
}

VOID printBlockMemoryState(DWORD state){
	char* str = (char*)malloc(128);
	switch (state){
	case MEM_COMMIT:
		str = "MEM_COMMIT";
		break;
	case MEM_FREE:
		str = "MEM_FREE";
		break;
	case MEM_RESERVE:
		str = "MEM_RESERVE";
		break;
	default:
		str = "?";
		break;
	}
	printf("%s", str);
}

VOID printRegionType(DWORD type){

	char* str = (char*)malloc(128);
	switch (type){
	case MEM_IMAGE:
		str = "MEM_IMAGE";
		break;
	case MEM_MAPPED:
		str = "MEM_MAPPED";
		break;
	case MEM_PRIVATE:
		str = "MEM_PRIVATE";
		break;
	default:
		str = "?";
		break;
	}
	printf("%s", str);
}


VOID printRegionInfo(DWORD nRegion, MEMORY_BASIC_INFORMATION memInfo, HANDLE hProc){
	printf("\n\nREGION # %i\n", nRegion);
	DWORD enderecoInicio = memInfo.BaseAddress;
	DWORD enderecoFim = enderecoInicio + memInfo.RegionSize;

	//endereço início e fim da região;
	printf("\nEndereco de inicio = %x ", enderecoInicio);

	//tipo de acesso (read, write, execute);
	printf("\nEndereco de fim = %x ", enderecoFim);

	// Tipo de acesso
	printf("\nTipo de Acesso = ");
	DWORD accessType = memInfo.AllocationProtect;
	printBlockAccessType(accessType);

	//protecção com que a região foi alocada inicialmente
	printf("\nProteccao com que a regiao foi alocada inicialmente = ");
	DWORD allocatedProtec = memInfo.Protect;
	printBlockAccessType(allocatedProtec);

	printf("\nEstado das paginas dentro da regiao = ");
	DWORD memState = (DWORD)memInfo.State;
	printBlockMemoryState(memState);

	// tipo das páginas dentro da região (image, mapped, private)
	printf("\nTipo das paginas dentro da regiao = ");
	printRegionType(memInfo.Type);

	// no caso do tipo de páginas  ser image,  indicar o ficheiro com o caminho absoluto no sistema de ficheiros de onde teve origem.
	if (memInfo.Type == MEM_IMAGE){

		// Procurar o Module que tem um endereço válido (entre o endereço de início e endereço de fim)!
		HMODULE hMods[1024];
		DWORD cbNeeded;
		unsigned int i;
		if (EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded)){
			for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++){
				TCHAR szModName[MAX_PATH];

				if (hMods[i] >= enderecoInicio && hMods[i] <= enderecoFim)
				{
					// Obter o nome do caminho para o ficheiro
					if (GetModuleFileNameEx(hProc, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
					{
						_tprintf(TEXT("\n~~~~~ Caminho do ficheiro de origem = %s (0x%08X)\n"), szModName, hMods[i]);
					}
				}
			}
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	SYSTEM_INFO sysInfo;
	PPERFORMACE_INFORMATION ppinfo = malloc(sizeof(PERFORMACE_INFORMATION));
	_tprintf(_T("~~~~ Informação global do sistema ~~~~~"));

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	GetPerformanceInfo(ppinfo, sizeof(PERFORMACE_INFORMATION));
	/*
	Total de memória física existente
	Total de memória física disponível
	Total de memória virtual existente (física + page file)
	Total de memória virtual disponível*/

	printf("\nTotal de memoria fisica existente: %llu KiB = %llu MiB = %llu GiB", status.ullTotalPhys / KiloB,
		status.ullTotalPhys / MegaB, status.ullTotalPhys / GB);

	printf("\nTotal de memoria fisica disponivel: %llu KiB = %llu MiB = %llu GiB", status.ullAvailPhys / KiloB,
		status.ullAvailPhys / MegaB, status.ullAvailPhys / GB);

	ULONGLONG totalVirtualMem = ppinfo->PageSize * (ULONGLONG)ppinfo->CommitLimit;
	printf("\nTotal de memoria virtual existente (fisica + page file): %llu KiB = %llu MiB = %llu GiB", totalVirtualMem / KiloB,
		totalVirtualMem / MegaB, totalVirtualMem / GB);

	printf("\nTotal de memoria virtual disponivel: %llu KiB = %llu MiB = %llu GiB", status.ullAvailVirtual / KiloB,
		status.ullAvailVirtual / MegaB, status.ullAvailVirtual / GB);

	printf("\nClique em qualquer tecla para ver a informação local do processo...");
	getchar();
	_tprintf(_T("\n\n~~~~ Informação local do processo ~~~~~"));

	PROCESS_MEMORY_COUNTERS_EX proc_mem_counters;
	HANDLE hProc = NULL;

	if (argc == 2){ // Foi passado o ID de um processo
		DWORD pId = argv[1];
		_tprintf(_T("\nProcess ID = %u\n"), pId);
		hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
	}
	else{
		hProc = GetCurrentProcess();
		printf("\nProcess ID = %u", GetCurrentProcessId());
	}

	GetProcessMemoryInfo(hProc, &proc_mem_counters, sizeof(proc_mem_counters));

	// • Total de espaço de endereçamento existente
	printf("\nTotal de espaco de enderecamento existente: %llu KiB = %llu MiB",
		(proc_mem_counters.QuotaPagedPoolUsage + proc_mem_counters.QuotaNonPagedPoolUsage) / KiloB,
		(proc_mem_counters.QuotaPagedPoolUsage + proc_mem_counters.QuotaNonPagedPoolUsage) / MegaB);

	//• Total de espaço de endereçamento disponivel
	printf("\nTotal de espaco de enderecamento disponivel: %llu KiB = %llu MiB",
		proc_mem_counters.QuotaNonPagedPoolUsage / KiloB,
		proc_mem_counters.QuotaNonPagedPoolUsage / MegaB);

	//• Dimensao do working set
	printf("\nDimensao do Working Set: %llu KiB = %llu MiB",
		proc_mem_counters.WorkingSetSize / KiloB, proc_mem_counters.WorkingSetSize / MegaB);

	GetSystemInfo(&sysInfo);

	printf("\nPage Size = %d bytes = %d KiB \n",
		sysInfo.dwPageSize, sysInfo.dwPageSize / 1024);

	MEMORY_BASIC_INFORMATION memInfo;
	char * lpAddress = 0;
	DWORD nRegion = 1;
	while (VirtualQueryEx(hProc, lpAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)) != 0){
		if (memInfo.State == MEM_FREE) continue;
		lpAddress = (char*)memInfo.BaseAddress + memInfo.RegionSize; 
		printRegionInfo(nRegion, memInfo, hProc);
		nRegion++;
	}
	PressToContinue("\ncontinuar...");

	CloseHandle(hProc);
	return 0;
}