#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>
#include "Exercicio5DLL.h"

BOOL MEMINFOAPI GetSystemMemInfo(_Out_ PSYSMEMINFO pSysMemInfo){
	printf("GetSystemMemInfo from DLL project was called!\n");
	PERFORMANCE_INFORMATION pInfo;

	if (!GetPerformanceInfo(&pInfo, sizeof(pInfo))){
		printf("GetPerformanceInfo returned with error.\n");
		return FALSE;
	}

	pSysMemInfo->PhysTotal = pInfo.PhysicalTotal * pInfo.PageSize;
	pSysMemInfo->PhysAvailable = pInfo.PhysicalAvailable * pInfo.PageSize;
	pSysMemInfo->VirtualTotal = (ULONGLONG)pInfo.CommitLimit * pInfo.PageSize;
	pSysMemInfo->VirtualAvailable = (ULONGLONG)pInfo.CommitTotal * pInfo.PageSize;
	pSysMemInfo->VirtualAvailable = pSysMemInfo->VirtualTotal - pSysMemInfo->VirtualAvailable;

	return TRUE;
}

BOOL MEMINFOAPI GetProcMemInfo(_In_ DWORD dwProcId, _Out_ PPROCMEMINFO pProcMemInfo){
	MEMORYSTATUSEX memStatus;
	PROCESS_MEMORY_COUNTERS procMemCounters;
	HANDLE hProcess;

	memStatus.dwLength = sizeof (memStatus);

	if (!GlobalMemoryStatusEx(&memStatus)){
		printf("GlobalMemoryStatusEx returned with error.\n");
		return FALSE;
	}

	pProcMemInfo->UllTotalVirtual = memStatus.ullTotalVirtual;
	pProcMemInfo->UllAvailVirtual = memStatus.ullAvailVirtual;

	if (dwProcId == 0)
		dwProcId = GetCurrentProcessId();

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);

	if (hProcess == NULL){
		printf("Couldn't open the process.\n");
		return FALSE;
	}

	if (!GetProcessMemoryInfo(hProcess, &procMemCounters, sizeof(procMemCounters))){
		printf("GetProcessMemoryInfo returned with error.\n");
		return FALSE;
	}

	pProcMemInfo->WorkingSetSize = procMemCounters.WorkingSetSize;

	return TRUE;
}

VOID MEMINFOAPI PrintProcVirtualAddress(_In_ DWORD dwProcId){
	MEMORYSTATUSEX memStatus;
	MEMORY_BASIC_INFORMATION memBasicInfo;
	HMODULE hModule;
	LPDWORD lpcbNeeded;
	HANDLE hProcess;

	BOOL isFree = 0;
	DWORD i, count = 0, imageCount = 0, mappedCount = 0, privateCount = 0, freeCount = 0;
	SIZE_T regionSize = 0;
	LPCVOID lpAddress = 0;
	PVOID endAddress = 0;
	TCHAR buffer[MAX_PATH];
	CHAR * state = "", *type = "";

	if (dwProcId == 0)
		dwProcId = GetCurrentProcessId();

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);

	if (hProcess == NULL){
		printf("Couldn't open the process.\n");
		return;
	}

	memStatus.dwLength = sizeof (memStatus);

	if (!GlobalMemoryStatusEx(&memStatus)){
		printf("GlobalMemoryStatusEx returned with error.\n");
		return;
	}

	printf("Identificacao das regioes:");

	for (i = 0; i < memStatus.ullTotalVirtual; i += memBasicInfo.RegionSize){
		if (!VirtualQueryEx(hProcess, endAddress, &memBasicInfo, sizeof(MEMORY_BASIC_INFORMATION))){
			printf("VirtualQueryEx returned with error.\n");
			return;
		}

		regionSize = memBasicInfo.RegionSize;
		endAddress = (PVOID)((SIZE_T)memBasicInfo.BaseAddress + memBasicInfo.RegionSize);

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
					return;
				}

				if (!GetModuleFileName(hModule, buffer, MAX_PATH)){
					printf("GetModuleFileName returned with error.\n");
					return;
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
}