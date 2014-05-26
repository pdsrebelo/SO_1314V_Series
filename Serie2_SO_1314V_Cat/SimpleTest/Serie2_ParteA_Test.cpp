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

VOID uselessFunction(UT_ARGUMENT arg){
	ULONG c = 0;
	while (c < (ULONG)arg){
		HANDLE h2 = UtCreate(uselessFunction, NULL);
		int res = UtJoin(h2);
		printf("\n\n -> Expected = %d ... Returned = %d \n", 0, res);
		_ASSERTE(res == 0);
		c++;
	}
}

VOID auxFunction_for_Ex1(UT_ARGUMENT arg){

	int badResult = UtJoin(testerInvalidHandle); // Expected return = -1
	printf("\n\n -> Expected = %d ... Returned = %d \n", -1, badResult);
	_ASSERTE(badResult == -1);

	int anotherBadResult = UtJoin(NULL); // Expected return = -1
	printf("\n\n -> Expected = %d ... Returned = %d \n", -1, anotherBadResult);
	_ASSERTE(anotherBadResult == -1);

	int goodResult = UtJoin(testerHandle); // Expected return = 0
	printf("\n\n -> Expected = %d ... Returned = %d \n", 0, goodResult);
	_ASSERTE(goodResult == 0);
}

void EX1_TEST() {
	_tprintf(_T("\n :::  *** -- Starting EXERCICIO 1 (UtJoin function) TEST -- ***  ::: \n"));
	UtCreate(auxFunction_for_Ex1, NULL);
	testerHandle = UtCreate(uselessFunction, (UT_ARGUMENT)5);
	UtRun();
	_tprintf(_T("\n\n :::  *** -- EXERCICIO 1 - TEST COMPLETED -- ***  ::: \n"));
}


/***
SERIE 2 - PARTE A - TESTS FOR Ex.2
***/

void auxFunction_for_Ex2(UT_ARGUMENT arg){

}

void EX2_TEST() {
	_tprintf(_T("\n :::  *** -- Starting EXERCICIO 2 (UtSleep function) TEST -- ***  ::: \n"));

	UtCreate(auxFunction_for_Ex2, (UT_ARGUMENT)NULL);
	//UtCreate(UtSleepHelper, (VOID *)NULL);  // Doesn't receive args
	// TODO

	UtRun();
	_tprintf(_T("\n\n :::  *** -- EXERCICIO 2 - TEST COMPLETED -- ***  ::: \n"));
}


/***
SERIE 2 - PARTE A - TESTS FOR Ex.3
***/

void EX3_TEST() {
	_tprintf(_T("\n :::  *** -- Starting EXERCICIO 3 (CONTEXT SWITCH TIME) TEST -- ***  ::: \n"));

	// TODO
	UtRun();
	_tprintf(_T("\n\n :::  *** -- EXERCICIO 3 - TEST COMPLETED -- ***  ::: \n"));
}
