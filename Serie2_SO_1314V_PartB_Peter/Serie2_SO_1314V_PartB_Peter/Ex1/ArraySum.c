#include <windows.h>
#include <process.h>    /* _beginthreadex, _endthreadex */
#include <stdio.h>
#include "CounterUtils.h"

/*
Deverá medir os tempos de execução do cálculo do somatório, no mínimo, por uma única thread, por
um número de threads correspondente ao número de processadores lógicos do sistema e, por último, por
um valor de threads 10 vezes superior ao número de processadores lógicos do sistema.
*/

#define MAX_NUMBERS (1024 * 1024) /* Considere um array de inteiros com dimensão na ordem dos MBytes. */

typedef struct {
	LONGLONG leftIdx;
	LONGLONG rightIdx;
} Indexes, * PIndexes;

HANDLE hThreads[40];
LONGLONG numbers[MAX_NUMBERS];
LONGLONG bigSum;
LARGE_INTEGER startTime;

void FillArray(){
	LONGLONG i;
	for (i = 0; i <= MAX_NUMBERS; i++)
		numbers[i] = i+1;
}

unsigned _stdcall CalcSum(void * params){
	LONGLONG i, sum = 0;
	PIndexes args = (PIndexes) params;

	for (i = args->leftIdx; i <= args->rightIdx; i++){
		sum += numbers[i];
	}

	//printf("Left Idx: %llu; Right Idx: %llu; Sum: %llu\n", args->leftIdx, args->rightIdx, sum);

	bigSum += sum;

	return 0;
}

DWORD GetNumberOfProcessors(){
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

void Test(DWORD numberOfThreads){
	LONGLONG i, step;

	step = MAX_NUMBERS / numberOfThreads;
	bigSum = 0;

	startTime = StartCounter();
	for (i = 0; i < numberOfThreads; i++){
		PIndexes args;
		args = (PIndexes)malloc(sizeof(args));
		args->leftIdx = i * step;
		args->rightIdx = (i * step) + (step - 1);
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, CalcSum, args, 0, NULL);
	}
}

int main(){
	FillArray();
	DWORD nProcs = GetNumberOfProcessors();

	/******************** TEST 1 - Using just one Processor ********************/
	printf("------ Test 1 - Using just one Processor ------\n");
	Test(1);
	WaitForMultipleObjects(1, hThreads, TRUE, INFINITE);

	printf("\nThe sum is: %llu\nTime spent calculating: %llu microseconds\n", bigSum, GetCounter(startTime));
	printf("-------------- Test 1 has ended ---------------\n\n");
	/**************************************************************************/

	/******************** TEST 2 - Using all the available Processors ********************/
	printf("------ Test 2 - Using all the available Processors ------\n");
	Test(nProcs);
	WaitForMultipleObjects(nProcs, hThreads, TRUE, INFINITE);

	printf("\nThe sum is: %llu\nTime spent calculating: %llu microseconds\n", bigSum, GetCounter(startTime));
	printf("-------------------- Test 2 has ended --------------------\n\n");
	/*************************************************************************************/

	/******************** TEST 3 - Using 10 times the number of Processors ********************/
	printf("------ Test 3 - Using 10 times the number of Processors ------\n");
	Test(nProcs * 10);
	WaitForMultipleObjects(nProcs * 10, hThreads, TRUE, INFINITE);

	printf("\nThe sum is: %llu\nTime spent calculating: %llu microseconds\n", bigSum, GetCounter(startTime));
	printf("---------------------- Test 3 has ended ----------------------\n\n");
	/******************************************************************************************/

	printf("Press any key to exit...\n");
	getchar();
	return 0;
}