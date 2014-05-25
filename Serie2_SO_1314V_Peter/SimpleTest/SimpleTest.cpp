// SimpleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "List.h"

/////////////////////////////////////////////
//
// CCISEL 
// 2007-2010
//
// UThread    Library First    Test
//
// Jorge Martins, 2011
////////////////////////////////////////////
#define DEBUG

#define MAX_THREADS 10


#include <crtdbg.h>
#include <stdio.h>

#include "ThreadSwitchTest.h"
#include "..\Include\Uthread.h"


///////////////////////////////////////////////////////////////
//															 //
// Test 1: N threads, each one printing its number M times   //
//															 //
///////////////////////////////////////////////////////////////

ULONG Test1_Count;

VOID Test1_Thread (UT_ARGUMENT Argument) {
	UCHAR Char;
	ULONG Index;
	Char = (UCHAR) Argument;	

	for (Index = 0; Index < 10000; ++Index) {
	    putchar(Char);
		
	    if ((rand() % 4) == 0) {
		    UtYield();
	    }
    }

	++Test1_Count;	 
}

VOID Test1 ()  {
	ULONG Index;
	
	Test1_Count = 0; 

	printf("\n :: Test 1 - BEGIN :: \n\n");

	for (Index = 0; Index < MAX_THREADS; ++Index) {
		UtCreate(Test1_Thread, (UT_ARGUMENT) ('0' + Index));
	}   

	UtRun();

	_ASSERTE(Test1_Count == MAX_THREADS);
	printf("\n\n :: Test 1 - END :: \n");
}

VOID ThreadJoinTest(UT_ARGUMENT Argument){
	ULONG tId = (ULONG)Argument;
	printf("\n :: Thread %d - BEGIN :: \n\n", tId);

	printf("\n :: Thread %d - END :: \n\n", tId);
}

VOID JoinTest(){
	ULONG i, nrOfThreads = 2;

	for (i = 0; i < nrOfThreads; i++){
		UtCreate(ThreadSleepTest, (UT_ARGUMENT)(i + 1));
	}

	UtRun();
}

VOID SleepTest(){
	ULONG i, nrOfThreads = 2;

	for (i = 0; i < nrOfThreads; i++){
		UtCreate(ThreadSleepTest, (UT_ARGUMENT)(i + 1));
	}

	UtRun();
}

VOID ThreadSleepTest(UT_ARGUMENT Argument){
	ULONG tId = (ULONG)Argument;
	DWORD sleepTime = (rand() % 100) * 100;         // sleepTime in the range 0 to 9900
	printf("\n :: Thread %d - BEGIN :: \n\n", tId);
	printf("\n :: Thread %d - Will sleep for %d miliseconds :: \n\n", tId, sleepTime);
	UtSleep(sleepTime);
	printf("\n :: Thread %d - END :: \n\n", tId);
}

int main(){
	UtInit();

	//JoinTest();
	//SleepTest();

	/*
	Escreva programas para determinar o tempo de comutação de threads na biblioteca UThread. Para                            
	a medição de tempos, utilize a função da Windows API GetTickCount.
	*/
	SwitchTest();

	printf("Press any key to finish");
	getchar();

	UtEnd();
	return 0;
}

/* Original main */
int main1 () {
	UtInit();
 
	Test1();

	printf("Press any key to finish");
	getchar();
		 
	UtEnd();
	return 0;
}