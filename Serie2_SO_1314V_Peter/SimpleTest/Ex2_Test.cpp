#include "stdafx.h"
#include "..\Include\Uthread.h"
#include "Ex3_Test.h"

DWORD count = 0;

VOID ThreadSleepTest(UT_ARGUMENT Argument){
	DWORD timeSlept = 0;
	ULONG tId = (ULONG)Argument;
	DWORD sleepTime = (rand() % 100) * 100;         // sleepTime in the range 0 to 9900
	printf("\n :: Thread %d - BEGIN :: \n\n", tId);
	printf("\n :: Thread %d - Will sleep for %d miliseconds :: \n\n", tId, sleepTime);
	timeSlept = UtSleep(sleepTime);
	printf("\n :: Thread %d - END (It actually slept for %d miliseconds) :: \n\n", tId, timeSlept);
	count++;
}

VOID OtherFunc(UT_ARGUMENT Argument){
	ULONG tId = (ULONG)Argument;
	printf("\n :: Thread %d - BEGIN :: \n\n", tId);
	for (;;){
		if (count == 2)
			break;
		UtYield();
	}
	printf("\n :: Thread %d - END :: \n\n", tId);
}

VOID SleepTest(){
	ULONG i, nrOfThreads = 2;

	for (i = 0; i < nrOfThreads; i++){
		UtCreate(ThreadSleepTest, (UT_ARGUMENT)(i + 1));
	}

	UtCreate(OtherFunc, (UT_ARGUMENT)3);
	UtCreate(OtherFunc, (UT_ARGUMENT)4);
}