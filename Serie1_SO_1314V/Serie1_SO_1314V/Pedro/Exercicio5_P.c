#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>
#include "Exercicio5DLL.h"

#define KiB 1024
#define MiB (KiB * 1024)
#define GiB (MiB * 1024)

int main(){
	SYSMEMINFO sysMemInfo;
	PROCMEMINFO procMemInfo;

	GetSystemMemInfo(&sysMemInfo);
	printf("\n** Informacao global: **\n");
	printf("1. Total de memoria fisica existente: %d KiB (%.2f GB)\n", (sysMemInfo.PhysTotal / KiB), ((double)sysMemInfo.PhysTotal / GiB));
	printf("2. Total de memoria fisica disponivel: %d KiB (%.2f GB)\n", (sysMemInfo.PhysAvailable / KiB), ((double)sysMemInfo.PhysAvailable / GiB));
	printf("3. Total de memoria virtual existente (fisica + page file): %llu KiB (%.2f GB)\n", (sysMemInfo.VirtualTotal / KiB), ((double)sysMemInfo.VirtualTotal / GiB));
	printf("4. Total de memoria virtual disponivel: %llu KiB (%.2f GB)\n", (sysMemInfo.VirtualAvailable / KiB), ((double)sysMemInfo.VirtualAvailable / GiB));

	GetProcMemInfo(0, &procMemInfo);
	printf("\n** Informacao local do processo: **\n");
	printf("1. Total de espaco de enderecamento virtual existente: : %I64d KiB (%.2f GB)\n", (procMemInfo.UllTotalVirtual / KiB), ((double) procMemInfo.UllTotalVirtual / GiB));
	printf("2. Total de espaco de enderecamento virtual disponivel: : %I64d KiB (%.2f GB)\n", (procMemInfo.UllAvailVirtual / KiB), ((double) procMemInfo.UllAvailVirtual / GiB));
	printf("3. Dimensao do Working set: %u KiB\n", procMemInfo.WorkingSetSize / KiB);

	PrintProcVirtualAddress(0);

	printf("Press any key to finish...");
	getchar();
	return 0;
}
