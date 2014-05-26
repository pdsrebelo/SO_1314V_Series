#include "stdafx.h"
#include "..\Include\Uthread.h"
#include "Ex1_Test.h"

#define NR_OF_YIELDS 1000000
#define TO_NANO 1000

ULONG yieldCount;

VOID MainFunc(UT_ARGUMENT Argument){
	float bigSample;
	DWORD initTimer;
	ULONG tId = (ULONG)Argument;

	printf("\n :: Thread %d - BEGIN :: \n", tId);

	initTimer = GetTickCount();
	while (yieldCount < NR_OF_YIELDS){
		UtYield();
		yieldCount++;
	}
	bigSample = GetTickCount() - initTimer;

	bigSample = bigSample / NR_OF_YIELDS;

	printf("\n :: Thread %d - AVERAGE TIME TO DO A CONTEXT SWITCH %f NANO :: \n", tId, (bigSample * TO_NANO));
	printf("\n :: Thread %d - END :: \n\n", tId);
}

VOID AuxFunc(UT_ARGUMENT Argument){
	while (yieldCount < NR_OF_YIELDS){
		UtYield();
		yieldCount++;
	}
}

VOID SwitchTest(){
	UtCreate(MainFunc, (UT_ARGUMENT)1);
	UtCreate(AuxFunc, (UT_ARGUMENT)2);
}