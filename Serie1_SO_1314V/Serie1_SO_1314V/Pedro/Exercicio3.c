#include <stdio.h>
#include <Windows.h>
#include <tchar.h> 
#include <Psapi.h>

/*
3.	Realize um programa que apresente na consola a seguinte informação global do sistema e 
informação do processo em execução cujo process id é passado como argumento.

Informação global:
● Total de memória física existente;
● Total de memória física disponível;
● Total de memória virtual existente (física + page file);
● Total de memória virtual disponível.

Informação local do processo:
● Total de espaço de endereçamento virtual existente;
● Total de espaço de endereçamento virtual disponível;
● Dimensão do Working set;
● Identificação das regiões (conjunto de páginas com as mesmas características) que 
formam o espaço de endereçamento de um processo caracterizando­as de acordo com os 
seguintes itens:
	○ Endereço início e fim da região;
	○ Tipo de acesso (read, write, execute);
	○ Protecção com que a região foi alocada inicialmente;
	○ Estado das páginas dentro da região (commit, free, reserve);
	○ Tipo das páginas dentro da região (image, mapped, private);
	○ No caso do tipo de páginas ser image, indicar o ficheiro com o caminho absoluto 
	no sistema de ficheiros de onde teve origem.

Notas: 
1. Confirme os seus resultados com os resultados apresentados pelas ferramentas 
ResourceMonitor e VMMap.
2. Funções interessantes da API Windows: GetPerformanceInfo, GlobalMemoryStatusEx, 
GetProcessMemoryInfo, VirtualQueryEx, EnumProcessModulesEx
3. Algumas das funções anteriores implicam a ligação com a biblioteca psapi.lib e da 
utilização do ficheiro psapi.h.
*/

#define KiB 1024
#define MiB (KiB * 1024)
#define GiB (MiB * 1024)
#define BUFFER 1024

BOOL globalSysInfo(DWORD processId){
	PERFORMANCE_INFORMATION pInfo;
	MEMORYSTATUSEX memStatus;
	PROCESS_MEMORY_COUNTERS procMemCounters;
	MEMORY_BASIC_INFORMATION memBasicInfo;
	HMODULE hModule;
	LPDWORD lpcbNeeded;
	HANDLE hProcess;

	BOOL isFree = 0;
	DWORD i, count = 0, imageCount = 0, mappedCount = 0, privateCount = 0, freeCount = 0;
	SIZE_T physTotal = 0, physAvailable = 0, workingSetSize = 0, regionSize = 0;
	ULONGLONG virtualTotal = 0, virtualAvailable = 0, ullTotalVirtual = 0, ullAvailVirtual = 0;
	LPCVOID lpAddress = 0;
	PVOID endAddress = 0;
	wchar_t buffer[BUFFER];
	CHAR * state = "",  * type = "";

	printf("\nProcess ID: %d\n", processId);

	if (!GetPerformanceInfo(&pInfo, sizeof(pInfo))){
		printf("GetPerformanceInfo returned with error.\n");
		return FALSE;
	}

	physTotal = pInfo.PhysicalTotal * pInfo.PageSize;
	physAvailable = pInfo.PhysicalAvailable * pInfo.PageSize;
	virtualTotal = (ULONGLONG) pInfo.CommitLimit * pInfo.PageSize;
	virtualAvailable = (ULONGLONG) pInfo.CommitTotal * pInfo.PageSize;
	virtualAvailable = virtualTotal - virtualAvailable;

	printf("\n** Informacao global: **\n");
	printf("1. Total de memoria fisica existente: %d KiB (%.2f GB)\n", (physTotal / KiB), ((double) physTotal / GiB));
	printf("2. Total de memoria fisica disponivel: %d KiB (%.2f GB)\n", (physAvailable / KiB), ((double) physAvailable / GiB));
	printf("3. Total de memoria virtual existente (fisica + page file): %llu KiB (%.2f GB)\n", (virtualTotal / KiB), ((double) virtualTotal / GiB));
	printf("4. Total de memoria virtual disponivel: %llu KiB (%.2f GB)\n", (virtualAvailable / KiB), ((double) virtualAvailable / GiB));

	/* The size of the structure, in bytes. You must set this member before calling GlobalMemoryStatusEx. */
	memStatus.dwLength = sizeof (memStatus);

	if (!GlobalMemoryStatusEx(&memStatus)){
		printf("GlobalMemoryStatusEx returned with error.\n");
		return FALSE;
	}

	/* The size of the user-mode portion of the virtual address space of the calling process, in bytes.
	This value depends on the type of process, the type of processor, and the configuration of the operating system.
	For example, this value is approximately 2 GB for most 32-bit processes on an x86 processor and approximately 3 GB for 32-bit processes
	that are large address aware running on a system with 4-gigabyte tuning enabled. */
	ullTotalVirtual = memStatus.ullTotalVirtual;

	/* The amount of unreserved and uncommitted memory currently in the user-mode portion of the virtual address space of the calling process, in bytes. */
	ullAvailVirtual = memStatus.ullAvailVirtual;

	/*If the function succeeds, the return value is an open handle to the specified process.
	If the function fails, the return value is NULL.To get extended error information, call GetLastError. */
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

	if (hProcess == NULL){
		printf("Couldn't open the process.\n");
		return FALSE;
	}

	if (!GetProcessMemoryInfo(hProcess, &procMemCounters, sizeof(procMemCounters))){
		printf("GetProcessMemoryInfo returned with error.\n");
		return FALSE;
	}

	/* The current working set size, in bytes. */
	workingSetSize = procMemCounters.WorkingSetSize;

	printf("\n** Informacao local do processo: **\n");
	printf("1. Total de espaco de enderecamento virtual existente: : %I64d KiB (%.2f GB)\n", (ullTotalVirtual / KiB), ((double) ullTotalVirtual / GiB));
	printf("2. Total de espaco de enderecamento virtual disponivel: : %I64d KiB (%.2f GB)\n", (ullAvailVirtual / KiB), ((double) ullAvailVirtual / GiB));
	printf("3. Dimensao do Working set: %u KiB\n", workingSetSize/KiB);
	printf("4. Identificacao das regioes:");

	for (i = 0; i < ullTotalVirtual; i += memBasicInfo.RegionSize){
		/* The return value is the actual number of bytes returned in the information buffer.
		If the function fails, the return value is zero. To get extended error information, call GetLastError.
		Possible error values include ERROR_INVALID_PARAMETER. */
		if (!VirtualQueryEx(hProcess, endAddress, &memBasicInfo, sizeof(MEMORY_BASIC_INFORMATION))){
			printf("VirtualQueryEx returned with error.\n");
			return FALSE;
		} 

		regionSize = memBasicInfo.RegionSize;
		endAddress = (PVOID) ((SIZE_T) memBasicInfo.BaseAddress + memBasicInfo.RegionSize);

		printf("\n-----------------------------------------\n");
		printf("Block #%d\n", ++count);
		printf("Endereco Inicio: 0x%x\n", memBasicInfo.BaseAddress);
		printf("Endereco Fim: 0x%x\n", endAddress);

		if (memBasicInfo.State == 0x1000){
			isFree = 0;
			state = "Commit Pages";
		}
		else if (memBasicInfo.State == 0x2000){
			isFree = 0;
			state = "Reserved Pages";
		}
		else{ //if (memBasicInfo.State == 0x10000){
			isFree = 1;
			state = "Free Pages";
			freeCount++;
		}

		printf("Estado: %s\n", state);

		if (!isFree){
			if (memBasicInfo.Type == 0x1000000){
				type = "Image";

				if (!EnumProcessModulesEx(hProcess, &hModule, sizeof(hModule), &lpcbNeeded, LIST_MODULES_ALL)){
					printf("EnumProcessModulesEx returned with error.\n");
					return FALSE;
				}

				if (!GetModuleFileName(hModule, buffer, BUFFER)){
					printf("GetModuleFileName returned with error.\n");
					return FALSE;
				}

				printf("Caminho absoluto: %ls\n", buffer);
				imageCount++;
			}
			else if (memBasicInfo.Type == 0x40000){
				type = "Mapped";
				mappedCount++;
			}
			else{
				type = "Private";
				privateCount++;
			}

			printf("Tipo: %s\n", type);

			printf("Tipo Acesso: %d\n", memBasicInfo.AllocationProtect);
		}
		printf("-----------------------------------------");
	}

	printf("\n\nTotal: [Image]: %d; [Mapped]: %d; [Private]: %d; [Free]: %d\n", imageCount, mappedCount, privateCount, freeCount);
	return TRUE;
}


int main2(){
	DWORD processID;
	printf("Introduza o ID de um processo a correr (ou 0 para analisar o proprio processo):\n");
	scanf_s("%d", &processID);

	BOOL success = globalSysInfo(processID == 0 ? GetCurrentProcessId() : processID);
	printf("\n------ Program terminated with %s ------", (success ? "success." : "error."));

	getchar();
	printf("\n\nPress any key to exit...\n");

	return 0;
}