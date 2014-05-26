// SimpleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "USynch.h"
#include "List.h"
#include "UThread.h"
#include <crtdbg.h>
#include <stdio.h>
#include "Serie2_ParteA_Test.h"
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

///////////////////////////////////////////////////////////////
//															 //
// Test 1: N threads, each one printing its number M times //
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

int main () {
	UtInit();
 
	// The UThread test that the teacher gave us
	Test1();
	
	// The tests created for Serie 2 - Parte A
	EX1_TEST();
	//EX2_TEST();
	//EX3_TEST();
	
	getchar();
	return 0;
}


