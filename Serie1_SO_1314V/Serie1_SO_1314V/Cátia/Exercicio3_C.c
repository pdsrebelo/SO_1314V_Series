#include <windows.h>
#include <stdio.h>
#include <Psapi.h>
#include <tchar.h>

#include "Exercicio5_MEMINFO_DLL_C.h"

#define KiloB 1024
#define MegaB 1048576
#define GigaB 1073741824

/*
To use the functionality from the class library in the app:

1-	After you create a console app, an empty program is created for you. (this file "Exercicio3_C.c")

2-	To use what you created in the DLL, you must reference it. To do this, select the DLL project in Solution Explorer, and then on the menu bar, 
	choose Project, References. In the Property Pages dialog box, expand the Common Properties node, select Framework and References, 
	and then choose the Add New Reference button. For more information about the References dialog box, see Framework and References, 
	Common Properties, <Projectname> Property Pages Dialog Box.

3-	The Add Reference dialog box lists the libraries that you can reference. 
	The Project tab lists the projects in the current solution and any libraries that they contain. 
	On the Projects tab, select the check box next to the DLL you want to use ("Exercicio5_MEMINFO_DLL_C.h"), and then choose the OK button.

4-	To reference the header files of the DLL, you must modify the included directories path. 
	To do this, in the Property Pages dialog box, expand the Configuration Properties node, expand the C/C++ node, and then select General. 
	Next to Additional Include Directories, specify the path of the location of the MathFuncsDll.h header file. 
	You can use a relative path—for example, ..\MathFuncsDll\—then choose the OK button.

5-	You can now use DLL.
*/
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
		DWORD cbNeeded; //The number of bytes required to store all module handles in the lphModule array.
		unsigned int i;
		if (EnumProcessModules(hProc, modules, sizeof(modules), &cbNeeded)){
			for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++){
				CHAR fileNameBuffer[MAX_PATH];
				ULONG moduleAddr = (ULONG)modules[i];
				if (modules[i] >= enderecoInicio && modules[i] <= enderecoFim)
				{
					// Obter o nome do caminho para o ficheiro
					if (GetModuleFileName(modules[i], fileNameBuffer, sizeof(fileNameBuffer)))
						printf("\nCaminho do FICHEIRO de origem = %ls (0x%08X)\n", fileNameBuffer, modules[i]);
				}
			}
		}
	}
}

VOID getGlobalInformation(){
	PPERFORMACE_INFORMATION ppinfo = malloc(sizeof(PERFORMACE_INFORMATION));
	ULONGLONG totalVirtualMem, totalVirtualMemAvail, totalPhysMem, totalPhysMemAvail;

	printf("~~~~ INFORMACAO GLOBAL ~~~~~");
	if (GetPerformanceInfo(ppinfo, sizeof(PERFORMACE_INFORMATION)) == FALSE){
		printf("\nERROR: %s", GetLastError()); return;
	}

	totalVirtualMem = ppinfo->PageSize * (ULONGLONG)ppinfo->CommitLimit;
	totalVirtualMemAvail = ppinfo->PageSize * (ULONGLONG)ppinfo->CommitTotal;
	totalPhysMem = ppinfo->PageSize * ppinfo->PhysicalTotal;
	totalPhysMemAvail = ppinfo->PageSize * ppinfo->PhysicalAvailable;

	printf("\nTotal de memoria fisica existente: %llu KiB = %llu MiB = %.2f GiB", totalPhysMem / KiloB, totalPhysMem / MegaB, totalPhysMem / GigaB);
	printf("\nTotal de memoria fisica disponivel: %llu KiB = %llu MiB = %.2f GiB", totalPhysMemAvail / KiloB, totalPhysMemAvail / MegaB, totalPhysMemAvail / GigaB);
	printf("\nTotal de memoria virtual existente (fisica + page file): %llu KiB = %llu MiB = %.2f GiB", totalVirtualMem / KiloB, totalVirtualMem / MegaB, totalVirtualMem / GigaB);
	printf("\nTotal de memoria virtual disponivel: %llu KiB = %llu MiB = %.2f GiB", totalVirtualMemAvail / KiloB, totalVirtualMemAvail / MegaB, totalVirtualMemAvail / GigaB);
	printf("\nClique em qualquer tecla para ver a informação local do processo..."); getchar();
}

VOID getProcessInformation(DWORD processID){
	PPERFORMACE_INFORMATION ppinfo = malloc(sizeof(PERFORMACE_INFORMATION));
	HANDLE hProc = NULL;
	MEMORYSTATUSEX status;
	MEMORY_BASIC_INFORMATION memInfo;
	PROCESS_MEMORY_COUNTERS procMemCtr;
	CHAR * lpAddress = 0;
	DWORD nRegion = 1;

	printf("\n\n~~~~ INFORMACAO LOCAL DO PROCESSO ~~~~~");

	status.dwLength = sizeof (status);

	if (hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID) == NULL ||
		!GetPerformanceInfo(ppinfo, sizeof(PERFORMACE_INFORMATION)) || !GlobalMemoryStatusEx(&status) ||
		!GetProcessMemoryInfo(hProc, &procMemCtr, sizeof(PROCESS_MEMORY_COUNTERS))){
		printf("ERROR: %s\n", GetLastError());
		return;
	}

	printf("\nProcess ID = %u", GetProcessId(hProc));
	printf("\nTamanho de pagina = %d bytes = %d KiB", ppinfo->PageSize, ppinfo->PageSize / KiloB);
	printf("\nTotal de espaco de enderecamento virtual existente: %llu KiB = %llu MiB", status.ullTotalVirtual / KiloB, status.ullTotalVirtual / MegaB);
	printf("\nTotal de espaco de enderecamento virtual disponivel: %llu KiB = %llu MiB", status.ullAvailVirtual / KiloB, status.ullAvailVirtual / MegaB);
	printf("\nDimensao do Working Set: %llu KiB = %llu MiB", procMemCtr.WorkingSetSize / KiloB, procMemCtr.WorkingSetSize / MegaB);

	while (VirtualQueryEx(hProc, lpAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)) != 0){
		lpAddress = (char*)memInfo.BaseAddress + memInfo.RegionSize;
		if (memInfo.State == MEM_FREE)
			continue;
		printRegionInfo(nRegion, memInfo, hProc);
		nRegion++;
	}
	printf("\n\nClique em qualquer tecla para terminar..."); getchar();
	CloseHandle(hProc);
}

/*
int main(){
	DWORD processID = 0;
	PROCMEMINFO_C procMemInfo;
	SYSMEMINFO_C sysMemInfo;

	printf("\nEnter the process ID: (or 0 for current process) ");
	scanf_s("%x", &processID);
	printf("\n* * * PROCESS ID = %d * * *\n", processID);
	if (processID == 0){
		processID = GetCurrentProcessId();
		printf("\nDefault ID (current process) = %d", processID);
	}
	
	//* ***NEW*** 
	// Using the DLL's functions
	C_GetSystemMemInfo(&sysMemInfo);
	if (!C_GetProcMemInfo(processID, &procMemInfo) || !C_PrintProcVirtualAddress(processID)){
		printf("\nAN ERROR OCCURRED!", GetLastError());
		return 0;
	}
	//* * * * * * * 
	
	//* ***OLD*** 
	// Calling local functions (without using the DLL)
	//getGlobalInformation();
	//getProcessInformation(processID);
	//* * * * * * * 
	printf("\nSYSTEM MEMORY INFO: ");
	printf("\nTotal Physical Memory = %llu KiB = %llu MiB = %.2f GiB", sysMemInfo.totalPhysicalMemory / KiloB, sysMemInfo.totalPhysicalMemory / MegaB, (double)sysMemInfo.totalPhysicalMemory / GigaB);
	printf("\nAvailable Physical Memory = %llu KiB = %llu MiB = %.2f GiB", sysMemInfo.availablePhysicalMemory/KiloB, sysMemInfo.availablePhysicalMemory/MegaB, (double)sysMemInfo.availablePhysicalMemory/GigaB);
	printf("\nTotal Virtual Memory = %llu KiB = %llu MiB = %.2f GiB", sysMemInfo.totalVirtualMemory / KiloB, sysMemInfo.totalVirtualMemory / MegaB, (double)sysMemInfo.totalVirtualMemory / GigaB);
	printf("\nAvailable Virtual Memory = %llu Kib = %llu MiB = %.2f GiB", sysMemInfo.availableVirtualMemory / KiloB, sysMemInfo.availableVirtualMemory / MegaB, (double)sysMemInfo.availableVirtualMemory / GigaB);
	printf("\n* * * * * * * * * * * * * * * * * * * * * * * * ");
	printf("\n\nPROCESS MEMORY INFO: ");
	printf("\nProcess ID = %d", procMemInfo.processId); 
	printf("\nPage Size = %s bytes = %llu KiB = %llu MiB", procMemInfo.pageSize, procMemInfo.pageSize / KiloB, procMemInfo.pageSize / MegaB);
	printf("\nWorking Set Size = %d bytes = %llu KiB = %llu MiB", procMemInfo.workingSetSize, procMemInfo.workingSetSize / KiloB, procMemInfo.workingSetSize / MegaB);
	printf("\nTotal Virtual Space = %llu KiB = %llu MiB = %.2f GiB", procMemInfo.totalVirtualSpace/KiloB, procMemInfo.totalVirtualSpace/MegaB, (double)procMemInfo.totalVirtualSpace/GigaB);
	printf("\nAvailable Virtual Space = %llu KiB = %llu MiB = %.2f GiB", procMemInfo.availableVirtualSpace / KiloB, procMemInfo.availableVirtualSpace / MegaB, procMemInfo.availableVirtualSpace/GigaB);
	printf("\n* * * * * * * * * * * * * * * * * * * * * * * * ");
	printf("\nEND.");
	return 1;
}
*/
