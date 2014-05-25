#include "stdafx.h"
#include "..\Include\Uthread.h"
#include "Ex2_Test.h"

#define ITERATIONS 1000000
HANDLE t1, t2, t3, t4;

VOID ThreadJoinTest(UT_ARGUMENT Argument){
	DWORD utJoinResult;
	ULONG tId = (ULONG)Argument;
	printf("\n :: Thread %d - BEGIN :: \n", tId);

	// utJoinResult = UtJoin(t4); // It should return -1; t4 is not a valid handle.
	utJoinResult = UtJoin(t4);

	printf("\n :: Thread %d - UtJoin returned: %d :: \n", tId, utJoinResult);

	printf("\n :: Thread %d - END :: \n\n", tId);
}

VOID ReallyLongFunc(UT_ARGUMENT Argument){
	DWORD i, count = 0;
	ULONG tId = (ULONG)Argument;

	printf("\n :: Thread %d - BEGIN :: \n", tId);

	for (i = 0; i < ITERATIONS; i++)
		count += i;

	printf("\n :: Thread %d - END :: \n\n", tId);
}

VOID JoinTest(){
	t1 = UtCreate(ThreadJoinTest, (UT_ARGUMENT)1);
	t2 = UtCreate(ReallyLongFunc, (UT_ARGUMENT)2);
	t3 = UtCreate(ReallyLongFunc, (UT_ARGUMENT)3);

	UtRun();
}