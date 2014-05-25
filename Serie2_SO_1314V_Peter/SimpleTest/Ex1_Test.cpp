#include "stdafx.h"
#include "..\Include\Uthread.h"
#include "Ex2_Test.h"

VOID ThreadJoinTest(UT_ARGUMENT Argument){
	ULONG tId = (ULONG)Argument;
	printf("\n :: Thread %d - BEGIN :: \n\n", tId);

	printf("\n :: Thread %d - END :: \n\n", tId);
}

VOID JoinTest(){
	ULONG i, nrOfThreads = 2;

	for (i = 0; i < nrOfThreads; i++){
		UtCreate(ThreadJoinTest, (UT_ARGUMENT)(i + 1));
	}

	UtRun();
}