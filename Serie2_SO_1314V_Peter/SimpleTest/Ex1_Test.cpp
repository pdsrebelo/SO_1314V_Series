#include "stdafx.h"
#include "..\Include\Uthread.h"
#include "Ex2_Test.h"

#define ITERATIONS 1000000
HANDLE t1, t2, t3, t4;

VOID ThreadJoinTest(UT_ARGUMENT Argument){
	DWORD utJoinResult, utJoinResult1;
	ULONG tId = (ULONG)Argument;
	printf("\n :: Thread %d - BEGIN (will wait for thread 3 to finish) :: \n", tId);

	utJoinResult = UtJoin(t4); // It should return -1; t4 is not a valid handle.
	printf("\n :: UtJoin returned %d :: \n", utJoinResult);

	utJoinResult1 = UtJoin(t3);
	printf("\n :: Thread %d - END (UtJoin returned %d) :: \n", tId, utJoinResult1);
}

VOID ReallyLongFunc(UT_ARGUMENT Argument){
	DWORD i, count = 0;
	ULONG tId = (ULONG)Argument;

	printf("\n :: Thread %d - BEGIN :: \n", tId);

	for (i = 0; i < ITERATIONS; i++){
		count += i;
		UtYield();
	}

	printf("\n :: Thread %d - END :: \n", tId);
}

VOID JoinTest(){
	t1 = UtCreate(ThreadJoinTest, (UT_ARGUMENT)1);
	t2 = UtCreate(ReallyLongFunc, (UT_ARGUMENT)2);
	t3 = UtCreate(ReallyLongFunc, (UT_ARGUMENT)3);
}