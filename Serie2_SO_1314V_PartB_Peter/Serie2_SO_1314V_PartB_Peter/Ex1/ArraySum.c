#include <windows.h>
#include <process.h>    /* _beginthreadex, _endthreadex */
#include <stdio.h>
#include "CounterUtils.h"

#define MAX_NUMBERS 8

typedef struct {
	DWORD leftIdx;
	DWORD rightIdx;
} Indexes, * PIndexes;

HANDLE hThreads[4];
DWORD numbers[MAX_NUMBERS] = { 1, 2, 3, 4, 5, 6, 7, 8 };
DWORD bigSum;
LARGE_INTEGER startTime1, startTime2, startTime3;

unsigned  _stdcall CalcSum(void * params){
	DWORD i, sum = 0;
	PIndexes args = (PIndexes) params;

	for (i = args->leftIdx; i <= args->rightIdx; i++){
		sum += numbers[i];
	}

	printf("Left Idx: %d; Right Idx: %d; Sum: %d\n", args->leftIdx, args->rightIdx, sum);

	bigSum += sum;

	return 0;
}

// Using all the available processors
void Test1(){
	SYSTEM_INFO si;
	DWORD i, step;

	printf("--- Test 1 is going to start ---\n");
	GetSystemInfo(&si);

	step = MAX_NUMBERS / si.dwNumberOfProcessors;
	bigSum = 0;

	startTime1 = StartCounter();
	for (i = 0; i < si.dwNumberOfProcessors; i++){
		PIndexes args;
		args = (PIndexes)malloc(sizeof(args));
		args->leftIdx = i * step;
		args->rightIdx = (i * step) + (step - 1);
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, CalcSum, args, 0, NULL);
	}
}

// Using only half of the available processors
void Test2(){
	SYSTEM_INFO si;
	DWORD i, step;

	printf("--- Test 2 is going to start ---\n");
	GetSystemInfo(&si);

	step = MAX_NUMBERS / (si.dwNumberOfProcessors / 2 );
	bigSum = 0;

	startTime2 = StartCounter();
	for (i = 0; i < si.dwNumberOfProcessors / 2 ; i++){
		PIndexes args;
		args = (PIndexes)malloc(sizeof(args));
		args->leftIdx = i * step;
		args->rightIdx = (i * step) + (step - 1);
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, CalcSum, args, 0, NULL);
	}
}

// Using only one processor
void Test3(){
	printf("--- Test 3 is going to start ---\n");

	bigSum = 0;

	startTime3 = StartCounter();
	PIndexes args;
	args = (PIndexes)malloc(sizeof(args));
	args->leftIdx = 0;
	args->rightIdx = MAX_NUMBERS;
	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, CalcSum, args, 0, NULL);
}

int main(){
	/******************** TEST 1 ********************/
	Test1();
	WaitForMultipleObjects(4, hThreads, TRUE, INFINITE);

	printf("\nThe sum is: %d\nTime spent calculating: %llu microseconds\n\n", bigSum, GetCounter(startTime1));
	/************************************************/


	/******************** TEST 2 ********************/
	Test2();
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);	

	printf("\nThe sum is: %d\nTime spent calculating: %llu microseconds\n\n", bigSum, GetCounter(startTime2));
	/************************************************/

	/******************** TEST 3 ********************/
	Test3();
	WaitForMultipleObjects(1, hThreads, TRUE, INFINITE);

	printf("\nThe sum is: %d\nTime spent calculating: %llu microseconds\n", bigSum, GetCounter(startTime3));
	/************************************************/

	printf("\nPress any key to exit...");
	getchar();
	return 0;
}