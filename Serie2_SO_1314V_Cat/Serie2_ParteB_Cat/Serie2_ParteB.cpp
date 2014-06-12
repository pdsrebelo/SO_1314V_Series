#include "Serie2_ParteB.h"
#include <math.h>
/*	EXERCÍCIO 1

1. Considere  um  array  de  inteiros  com  dimensão  na  ordem  dos  MBytes.  Pretende­-se  realizar  o 
somatório  de  todos  os  inteiros  do  array  de  uma  forma  eficiente  recorrendo  ao  paralelismo  de 
recursos  existentes  em  cada  sistema.  Uma  vez  que  a  operação  somatório  é  comutativa  e 
associativa,  o array poderá  ser dividido em  várias partes, estando o  cálculo do  somatório de cada 
parte  alocado  a  uma  thread.  Determine  para  o  seu  sistema,  qual  o  número  de  threads  que 
proporciona  melhores  tempos  de execução do  cálculo do  somatório. Apresente as medições dos 
tempos para as várias experiências.
*/
#define BIG_ARRAY_SIZE 3 * 1048576 // e.g. Array size: 3 Megabytes... // Note: 1 Megabyte = 1,048,576 bytes = (1024*1024) bytes
#define NUMBER_OF_TESTS 4
LONGLONG bigArray[BIG_ARRAY_SIZE];
LONGLONG resultArray[NUMBER_OF_TESTS];

typedef struct array_positions_for_threads{
	DWORD begin;
	DWORD end;
	DWORD resultIdx; // Index of resultArray - where each thread should add its sum
}ARRAY_POS,*PARRAY_POS;

unsigned int __stdcall partialArraySum(void * arg){ // Função que vai estar associada a cada thread, para calcular o somatório de uma parte do array
	DWORD i, start, end, accum = 0;
	PARRAY_POS position = (PARRAY_POS)arg;

	start = position->begin;
	end = position->end;

	for (i = start; i < end; i++){
		accum += bigArray[i];
	}
	resultArray[position->resultIdx] += accum;
	return resultArray[position->resultIdx];
}

void Ex1_ParallelArraySum(LONG nThreads, DWORD resultIdx) {

	DWORD i, arrayPositionsPerThread, extraPositions, 
		startIdx = 0; // Indexes of the array elements' positions 

	// Define the number of array positions that each thread will be responsible for counting
	arrayPositionsPerThread = BIG_ARRAY_SIZE / nThreads;// Divide the work by the threads
	extraPositions = BIG_ARRAY_SIZE % nThreads;			// The rest of the division result

	for (i = 0; i < nThreads; i++){
		DWORD begin, end;
		PARRAY_POS arrayPositions = (PARRAY_POS)malloc(sizeof(ARRAY_POS));

		begin = startIdx;
		end = begin + arrayPositionsPerThread;

		if (extraPositions>0){
			end += 1;
			extraPositions -= 1;
		}
		
		// Prepare the ARRAY_POS 
		arrayPositions->end = end;
		arrayPositions->begin = begin;
		arrayPositions->resultIdx = resultIdx;

		_beginthreadex(NULL, 0, partialArraySum, (VOID*)(arrayPositions), 0, NULL);

		startIdx = end;
	}

}

int main() {

	// First Test: nWorkingThreads = (2^0) * number of processors
	SYSTEM_INFO sysinfo;
	DWORD nCpu, nThreads;
	int i;

	// Initialize the array with the values to sum
	for (i = 0; i < BIG_ARRAY_SIZE; i++){
		bigArray[i] = rand() % 100 + 1;		// Random number between 1 and 100
	}

	GetSystemInfo(&sysinfo);
	nCpu = sysinfo.dwNumberOfProcessors;
	nThreads = nCpu;

	for (i = 0; i < NUMBER_OF_TESTS; i++){ // {(2^i) * number of processors} threads, per each CPU
		DWORD j, countNTimes = 100, totalNanos = 0, averageTime, initTime, finalTime, totalTime;

		nThreads = pow((double)2, i) * nCpu;

		resultArray[i] = 0;
		totalTime = 0;

		for (j = 0; j < countNTimes; j++){
			initTime = GetTickCount();
			
			Ex1_ParallelArraySum(nThreads, i);
			
			finalTime = GetTickCount();
			totalTime += (finalTime - initTime);
			//printf("  ... Took = %d ms\n", (finalTime - initTime));
		}
		averageTime = totalTime/countNTimes;
		printf("\n::::: Test With %d threads :::::", nThreads);
		printf("\nArray Sum = %ul\n", resultArray[i]);
		printf("Took (Average time calculated with %d tests) = %d ms\n", countNTimes, averageTime);
	}

	//TODO
	//	Determine  para  o  seu  sistema,  qual  o  número  de  threads  que 
	//	proporciona  melhores  tempos  de execução do  cálculo do  somatório.Apresente as medições dos 
	//	tempos para as várias experiências.
	getchar();
}