#include "Serie2_ParteB.h"
/*	EXERCÍCIO 1

1. Considere  um  array  de  inteiros  com  dimensão  na  ordem  dos  MBytes.  Pretende­-se  realizar  o 
somatório  de  todos  os  inteiros  do  array  de  uma  forma  eficiente  recorrendo  ao  paralelismo  de 
recursos  existentes  em  cada  sistema.  Uma  vez  que  a  operação  somatório  é  comutativa  e 
associativa,  o array poderá  ser dividido em  várias partes, estando o  cálculo do  somatório de cada 
parte  alocado  a  uma  thread.  Determine  para  o  seu  sistema,  qual  o  número  de  threads  que 
proporciona  melhores  tempos  de execução do  cálculo do  somatório. Apresente as medições dos 
tempos para as várias experiências.
*/
#define BIG_ARRAY_SIZE 1048576 // e.g. Array size: 3 Megabytes... // Note: 1 Megabyte = 1,048,576 bytes = (1024*1024) bytes

LONGLONG bigArray[BIG_ARRAY_SIZE];
LONGLONG arraySum;

typedef struct array_positions_for_threads{
	DWORD begin;
	DWORD end;
}ARRAY_POS,*PARRAY_POS;

unsigned int __stdcall partialArraySum(void * arg){ // Função que vai estar associada a cada thread, para calcular o somatório de uma parte do array

	// Now: Get the index values from the input arg!
	DWORD i, start, end, accum = 0;
	PARRAY_POS position = (PARRAY_POS)arg;
	start = position->begin;
	end = position->end;
	for (i = start; i < end; i++){
		accum += bigArray[i];
	}
	arraySum += accum;
	return arraySum;
}

void Ex1_ParallelArraySum() {

	//  Uma  vez  que  a  operação  somatório  é  comutativa  e 
	//  associativa, o array poderá  ser dividido em  várias partes, estando o  cálculo do  somatório de cada 
	//	parte  alocado  a  uma  thread.

	SYSTEM_INFO sysinfo;
	DWORD i, nCpu, nThreads, arrayPositionsPerThread, extraPositions;
	DWORD startIdx = 0; // Indexes of the array elements' positions 

	// Initialize the "arraySum" variable
	arraySum = 0;

	// Initialize the array with the values to sum
	for (i = 0; i < BIG_ARRAY_SIZE; i++){
		bigArray[i] = rand() % 100 + 1;		// Random number between 1 and 100
	}

	// Get the number of processor units
	GetSystemInfo(&sysinfo);
	nCpu = sysinfo.dwNumberOfProcessors;

	// Define the number of threads that we want to create
	nThreads = nCpu;

	// Define the number of array positions that each thread will be responsible for counting
	arrayPositionsPerThread = BIG_ARRAY_SIZE / nThreads;// Divide the work by the threads
	extraPositions = BIG_ARRAY_SIZE % nThreads;			// The rest of the division result

	// Starting with one thread per processor: ...

	for (i = 0; i < nThreads; i++){	// For each cpu
		DWORD begin, end;
		PARRAY_POS arrayPositions = (PARRAY_POS)malloc(sizeof(ARRAY_POS));

		begin = startIdx;
		end = begin + arrayPositionsPerThread - 1;

		if (extraPositions>0){
			end += 1;
			extraPositions -= 1;
		}
		startIdx = end + 1;

		// Create the thread that will get the sum of the elements in the specified array bounds
		// a struct is needed to pass the two arguments to the "partialArraySum" function
		arrayPositions->end = end;
		arrayPositions->begin = begin;
		_beginthreadex(NULL, 0, partialArraySum, (VOID*)(arrayPositions), 0, NULL);
	}
	//	Determine  para  o  seu  sistema,  qual  o  número  de  threads  que 
	//	proporciona  melhores  tempos  de execução do  cálculo do  somatório.Apresente as medições dos 
	//	tempos para as várias experiências.
}

int main() {
	// TODO: Test Ex1_ParallelArraySum
	Ex1_ParallelArraySum();
}