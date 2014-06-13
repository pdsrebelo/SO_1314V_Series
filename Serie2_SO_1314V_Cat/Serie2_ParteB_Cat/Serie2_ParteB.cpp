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

#define NUMBER_OF_TESTS 7

#define BIG_ARRAY_SIZE 3 * 1048576 // e.g. Array size: 3 Megabytes... // Note: 1 Megabyte = 1,048,576 bytes = (1024*1024) bytes
#define	MAX_NR_THREADS 64

LONGLONG bigArray [BIG_ARRAY_SIZE];
LONGLONG resultArray [NUMBER_OF_TESTS]; // the sum result is stored in this array - one array index for each test
LONGLONG averageTimes [NUMBER_OF_TESTS];  // array that stores the average times that took to run the program with different nr of threads
LONG threadNumber;


typedef struct array_positions_for_threads{
	DWORD begin;
	DWORD end;
	DWORD resultIdx; // Index of resultArray - where each thread should add its sum
}ARRAY_POS,*PARRAY_POS;

unsigned int __stdcall partialArraySum(void * arg){ // Função que vai estar associada a cada thread, para calcular o somatório de uma parte do array
	LONGLONG i, accum;
	PARRAY_POS position = (PARRAY_POS)arg;
	
	//printf("#%d ",threadNumber++); // Print the thread # for the test
	accum = 0;

	for (i = position->begin; i < position->end; i++)
		accum = accum + bigArray[i];

	resultArray[position->resultIdx] += accum;
	return resultArray[position->resultIdx];
}

DWORD Ex1_ParallelArraySum(LONG nThreads, DWORD index) {
	HANDLE workerThreads[MAX_NR_THREADS];
	DWORD i, arrayPositionsPerThread, extraPositions, 
		initialTime, totalTime,
		nextIdx = 0; // Indexes of the array elements' positions 

	// Define the number of array positions that each thread will be responsible for counting
	arrayPositionsPerThread = BIG_ARRAY_SIZE / nThreads;// Divide the work by the threads
	extraPositions = BIG_ARRAY_SIZE % nThreads;			// The rest of the division result
	
	// Reset the sum value stored in the array
	resultArray[index] = 0;
	threadNumber = 0;

	initialTime = GetTickCount();
	for (i = 0; i < nThreads; i++){
		DWORD begin, end;
		PARRAY_POS arrayPositions = (PARRAY_POS)malloc(sizeof(ARRAY_POS));

		begin = nextIdx;
		end = begin + arrayPositionsPerThread;

		if (extraPositions>0){
			end += 1;
			extraPositions -= 1;
		}
		
		// Prepare the ARRAY_POS 
		arrayPositions->resultIdx = index;
		arrayPositions->end = end;
		arrayPositions->begin = begin;
		
		workerThreads[i] = (HANDLE)_beginthreadex(NULL, 0, partialArraySum, (VOID*)(arrayPositions), 0, NULL);
		nextIdx = end;
	}
	WaitForMultipleObjects(nThreads, workerThreads, TRUE, INFINITE);
	totalTime = GetTickCount() - initialTime;
	//printf("\nTest#%d - Result=%d --->", index + 1, resultArray[index]);
	//printf("Time = %d micros\n", totalTime * 1000);
	return (totalTime * 1000);
}

int main() {
	DWORD nThreads, wThreadIdx;
	int i;

	// Initialize the array with the values to sum
	for (i = 0; i < BIG_ARRAY_SIZE; i++){
		bigArray[i] = rand() % 100 + 1;		// Random number between 1 and 100
	}

	// Start the tests
	for (i = 0; i < NUMBER_OF_TESTS; i++){ // (2^i) threads ... max: 2^10 « 1024
	
		DWORD j, countNTimes = 900, averageTime = 0;
		nThreads = pow((double)2, i); 
		printf("\n::::: Starting Test #%d With %d threads - Will run %d times :::::", i+1, nThreads, countNTimes);
	
		resultArray[i] = 0;
		
		for (j = 0; j < countNTimes; j++){
			averageTime += Ex1_ParallelArraySum(nThreads, i);
		}
		
		averageTime = averageTime/countNTimes;
		averageTimes[i] = averageTime;
		printf("\nAverage time for Test #%d = %d", i+1, averageTime);

	}

	// Now, compare the time results and see what is the best(faster) number of threads
	printf("\nPress any key to compare the tests' results...");
	getchar();
	DWORD bestTime = INFINITE, bestTest = -1;
	
	printf("\n::: Tests' times comparison :::");
	for (i = 0; i < NUMBER_OF_TESTS; i++){
		if (bestTime > averageTimes[i]){
			bestTime = averageTimes[i];
			bestTest = i;
		}
		printf("\nTest #%d = %d ns", i+1, averageTimes[i]);
	}
	printf("\n\nThe test with the best times was test #%d, with an average time of %d ns"
		,bestTest+1, bestTime);
	printf("\n\nDONE!\nPress any key to exit...");
	getchar();
}