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

#include "Ex1_Test.h"
#include "Ex2_Test.h"
#include "Ex3_Test.h"
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

int main(){
	UtInit();

	//JoinTest();		/* Ex1_Test */
	//SleepTest();	/* Ex2_Test */

	/*
	Escreva programas para determinar o tempo de comuta��o de threads na biblioteca UThread. Para                            
	a medi��o de tempos, utilize a fun��o da Windows API GetTickCount.
	*/
	SwitchTest();	/* Ex3_Test */

	UtRun();

	printf("\nPress any key to finish");
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