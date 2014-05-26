#include "stdafx.h"
#include "../Include/USynch.h"
#include "../Include/List.h"
#include "../Include/UThread.h"
/***
SERIE 2 - PARTE A - TESTS
***/

// auxFunction_for_Ex1 (This function is USELESS. Just using it for tests!)
VOID auxFunction_for_Ex1(UT_ARGUMENT countUpTo){ // counts up to the arg given; and returns the total time
	DWORD initTime = GetTickCount();
	DWORD totalTime;
	DWORD c = 0;
	for (;;){
		if (c < (DWORD)countUpTo)
			c++;
		else break;
	}

	totalTime = GetTickCount() - initTime;
}

void EX1_TEST() {
	_tprintf(_T("Starting EX1 UtJoin TEST.\n"));

	//TODO - Test Ex.1 = UtJoin(HANDLE thread)

	// Create the UThread
	HANDLE h1 = UtCreate(auxFunction_for_Ex1, (UT_ARGUMENT)10000); // Aux function = counts up to 10000 in a For Loop 
	HANDLE h2 = UtCreate(auxFunction_for_Ex1, (UT_ARGUMENT)10000);
	HANDLE h3 = UtCreate(auxFunction_for_Ex1, (UT_ARGUMENT)10000);

	//UtJoin(h1);
	//UtJoin(h2);
	//UtJoin(h3);

	UtRun();
}

void auxFunction_for_Ex2(UT_ARGUMENT arg){

}

void EX2_TEST() {
	_tprintf(_T("Starting EX2 UtSleep TEST.\n"));
	//TODO - Test Ex.2 = UtSleep(DWORD milis) & UtSleepHelper
	UtCreate(auxFunction_for_Ex2, (UT_ARGUMENT)NULL);

	//UtCreate(UtSleepHelper, (VOID *)NULL);  // Doesn't receive args

	UtRun();
}

void EX3_TEST() {

	_tprintf(_T("Starting EX3 TEST.\n"));

	UtRun();
}

/*
int main() {
	EX1_TEST();
	//EX2_TEST();
	//EX3_TEST();

	_tprintf(_T("Terminated all the tests.\n"));
	return 0;
}

*/