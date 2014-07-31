#include "stdafx.h"
#include "stdafx.h"
#include <crtdbg.h>
#include <stdio.h>
#include "../Include/USynch.h"
#include "../Include/List.h"
#include "../Include/UThread.h"

/***
SERIE 2 - PARTE A - TESTS FOR Ex.1
***/

HANDLE testerInvalidHandle, testerHandle;

DWORD uselessFunctionExample(UT_ARGUMENT arg){
	DWORD acc = 0, i = 0 ;
	DWORD input = (DWORD)arg;
	if (input>0){
		for (i = 0; i < input; i++){
			acc += i;
		}
	}
	return acc;
}

VOID uselessFunction(UT_ARGUMENT arg){
	ULONG c = 0;
	while (c < (ULONG)arg){
		HANDLE h2 = UtCreate((UT_FUNCTION)uselessFunctionExample, (UT_ARGUMENT)1000);
		int res = UtJoin(h2);
		printf("\n\n -> Expected = %d ... Returned = %d \n", 0, res);
		_ASSERTE(res == 0);
		c++;
	}
}

VOID auxFunction_for_Ex1(UT_ARGUMENT arg){
	DWORD currentTest = 1, totalTests = 3;

	int badResult = UtJoin(testerInvalidHandle); // Expected return = -1
	printf("\n\n -> Expected = %d ... Returned = %d \n", -1, badResult);
	_ASSERTE(badResult == -1);
	printf("\nPress any key to continue... (%d / %d Tests passed)", currentTest++, totalTests);
	getchar();

	int anotherBadResult = UtJoin(NULL); // Expected return = -1
	printf("\n\n -> Expected = %d ... Returned = %d \n", -1, anotherBadResult);
	_ASSERTE(anotherBadResult == -1);
	printf("\nPress any key to continue... (%d / %d Tests passed)", currentTest++, totalTests);
	getchar();

	int goodResult = UtJoin(testerHandle); // Expected return = 0
	printf("\n\n -> Expected = %d ... Returned = %d \n", 0, goodResult);
	_ASSERTE(goodResult == 0);
	printf("\nPress any key to continue... (%d / %d Tests passed)", currentTest++, totalTests);
	getchar();

}

void EX1_TEST() {
	UtInit();
	_tprintf(_T("\n :::  *** -- Starting EXERCICIO 1 (UtJoin function) TEST -- ***  ::: \n"));
	UtCreate(auxFunction_for_Ex1, NULL);
	testerHandle = UtCreate(uselessFunction, (UT_ARGUMENT)5);
	UtRun();
	_tprintf(_T("\n\n :::  *** -- EXERCICIO 1 - TEST COMPLETED -- ***  ::: \n"));
	UtEnd();
}


/***
SERIE 2 - PARTE A - TESTS FOR Ex.2
***/
DWORD currentEx2Test, totalEx2Tests;

// function that call UtSleep
void auxFunction_for_Ex2(UT_ARGUMENT time){
	DWORD minimumTime = (DWORD)time;	// Minimum time for the thread to Sleep
	DWORD totalTimeWaiting = UtSleep(minimumTime);	// Expected return: >= 4000
	printf("\n\n -> Expected: >= %d ... Returned: %d \n", minimumTime, totalTimeWaiting);
	_ASSERTE(totalTimeWaiting >= minimumTime);
	printf("\nPress any key to continue... (%d / %d Tests passed)", currentEx2Test++, totalEx2Tests);
	getchar();
}



void EX2_TEST() {
	totalEx2Tests = 4, currentEx2Test = 1;
	UtInit();
	_tprintf(_T("\n :::  *** -- Starting EXERCICIO 2 (UtSleep function) TEST -- ***  ::: \n"));

	// Wait more than 0 milliseconds
	UtCreate(auxFunction_for_Ex2, (UT_ARGUMENT)0);

	// Wait more than 2000 milliseconds
	UtCreate(auxFunction_for_Ex2, (UT_ARGUMENT)2000);

	// Wait more than 10000 milliseconds
	UtCreate(auxFunction_for_Ex2, (UT_ARGUMENT)10000);

	// Wait more than 13000 milliseconds
	UtCreate(auxFunction_for_Ex2, (UT_ARGUMENT)13000);

	UtRun();
	_tprintf(_T("\n\n :::  *** -- EXERCICIO 2 - TEST COMPLETED -- ***  ::: \n"));
	UtEnd();
}


/***
SERIE 2 - PARTE A - TESTS FOR Ex.3
***/
DWORD totalEx3Tests, currentEx3Test, ex3FunctionsFinished;
BOOL test3MustEnd;
void auxFunctionThatYields(UT_ARGUMENT arg){
	while (ex3FunctionsFinished<totalEx3Tests) {
		UtYield();
	}
}

VOID veryUselessTimeConsumingFunction(UT_ARGUMENT millis){
	DWORD totalTimeSlept = UtSleep((DWORD)millis);
	printf("\nExpected = %d ... Returned = %d",(DWORD)millis, totalTimeSlept);
	_ASSERTE(totalTimeSlept>=(DWORD)millis);
	printf("\nPress any key to continue... (%d / %d Tests passed)", currentEx3Test++, totalEx3Tests);
	getchar();
	ex3FunctionsFinished += 1;
}

void EX3_TEST() {
	UtInit();
	_tprintf(_T("\n :::  *** -- Starting EXERCICIO 3 (CONTEXT SWITCH TIME) TEST -- ***  ::: \n"));

	totalEx3Tests = 5; currentEx3Test = 1; ex3FunctionsFinished = 0;
	test3MustEnd = FALSE;

	// Create the thread that will wake the others in case they are ALL sleeping
	UtCreate(auxFunctionThatYields, (UT_ARGUMENT)NULL);

	UtCreate((UT_FUNCTION)veryUselessTimeConsumingFunction, (UT_ARGUMENT)1000);		//1 s
	UtCreate((UT_FUNCTION)veryUselessTimeConsumingFunction, (UT_ARGUMENT)2000);		//2 s
	UtCreate((UT_FUNCTION)veryUselessTimeConsumingFunction, (UT_ARGUMENT)5000);		//5 s
	UtCreate((UT_FUNCTION)veryUselessTimeConsumingFunction, (UT_ARGUMENT)10000);	//10 s
	UtCreate((UT_FUNCTION)veryUselessTimeConsumingFunction, (UT_ARGUMENT)12000);	//12 s

	UtRun();
	
	_tprintf(_T("\n\n :::  *** -- EXERCICIO 3 - TEST COMPLETED -- ***  ::: \n"));
	UtEnd();
}
