/////////////////////////////////////////////////////////////////
//
// CCISEL 
// 2007-2011
//
// UThread library:
//   User threads supporting cooperative multithreading.
//
// Authors:
//   Carlos Martins, João Trindade, Duarte Nunes, Jorge Martins
// 
#include <windows.h>
#include <malloc.h>
#include <tchar.h>
#include <ctype.h>
#include <stdio.h>
#include <strsafe.h>
#include "List.h"
#include "USynch.h"
#include <crtdbg.h>
#include "UThreadInternal.h"

//////////////////////////////////////
//
// UThread internal state variables.
//

//
// The number of existing user threads.
//
static
ULONG NumberOfThreads;

//
// The sentinel of the circular list linking the user threads that are
// currently schedulable. The next thread to run is retrieved from the
// head of this list.
//
static LIST_ENTRY ReadyQueue;

// SERIE 2 - PARTE A - EX 1 e EX 2
static LIST_ENTRY SleepyQueue;		// Circular FIFO list linking the UThreads that are currently sleeping

//
// The currently executing thread.
//
#ifndef UTHREAD_X64
static
#endif
PUTHREAD RunningThread;



//
// The user thread proxy of the underlying operating system thread. This
// thread is switched back in when there are no more runnable user threads,
// causing the scheduler to exit.
//
static
PUTHREAD MainThread;

////////////////////////////////////////////////
//
// Forward declaration of internal operations.
//

//
// The trampoline function that a user thread begins by executing, through
// which the associated function is called.
//
static
VOID InternalStart();


#ifdef UTHREAD_X64
//
// Performs a context switch from CurrentThread to NextThread.
// In x64 calling convention CurrentThread is in RCX and NextThread in RDX.
//
VOID __fastcall  ContextSwitch(PUTHREAD CurrentThread, PUTHREAD NextThread);

//
// Frees the resources associated with CurrentThread and switches to NextThread.
// In x64 calling convention  CurrentThread is in RCX and NextThread in RDX.
//
VOID __fastcall InternalExit(PUTHREAD Thread, PUTHREAD NextThread);

#else

static
VOID __fastcall ContextSwitch(PUTHREAD CurrentThread, PUTHREAD NextThread);

//
// Frees the resources associated with CurrentThread and switches to NextThread.
// __fastcall sets the calling convention such that CurrentThread is in ECX
// and NextThread in EDX.
//
static
VOID __fastcall InternalExit(PUTHREAD Thread, PUTHREAD NextThread);
#endif

////////////////////////////////////////
//
// UThread inline internal operations.
//

//
// Returns and removes the first user thread in the ready queue. If the ready
// queue is empty, the main thread is returned.
//
static
FORCEINLINE
PUTHREAD ExtractNextReadyThread() {
	return IsListEmpty(&ReadyQueue)
		? MainThread
		: CONTAINING_RECORD(RemoveHeadList(&ReadyQueue), UTHREAD, Link);
}

//
// Schedule a new thread to run
//

//
// Exercicio 3 - Serie 2 - Parte A - Catia Ormonde, nº36923
// This function was updated to get the time, in milliseconds, that it takes to switch from a running thread to another.
//
static
FORCEINLINE
VOID Schedule() {
	PUTHREAD NextThread;
	DWORD timeBeforeCall, timeAfterCall, totalTimeMillis;
	NextThread = ExtractNextReadyThread();

	if (NextThread->IsWaitingForCompletion){ // If the next thread to run has called UtJoin ('IsWaitingForCompletion = TRUE'), Activate it!
		UtActivate((HANDLE)NextThread);			// Once activated, the thread will be waiting for its end (the "finishedEvt" event set)
	}

	// Register the time before calling the ContextSwitch function
	timeBeforeCall = GetTickCount();

	ContextSwitch(RunningThread, NextThread);

	// Register the time after the call
	timeAfterCall = GetTickCount();

	((PUTHREAD)RunningThread)->State = Running; // The new thread has a new state = Running

	// The total time in milliseconds:
	totalTimeMillis = timeAfterCall - timeBeforeCall;

	printf("\nTotal time in millis = %d mHz/ms", totalTimeMillis);
}

///////////////////////////////
//
// UThread public operations.
//

//
// Initialize the scheduler.
// This function must be the first to be called. 
//
VOID UtInit() {
	InitializeListHead(&ReadyQueue);
	InitializeListHead(&SleepyQueue);
}

//
// Cleanup all UThread internal resources.
//
VOID UtEnd() {
	/* (this function body was intentionally left empty) */
}

//
// Run the user threads. The operating system thread that calls this function
// performs a context switch to a user thread and resumes execution only when
// all user threads have exited.
//
VOID UtRun() {
	UTHREAD Thread; // Represents the underlying operating system thread.

	//
	// There can be only one scheduler instance running.
	//
	_ASSERTE(RunningThread == NULL);

	//
	// At least one user thread must have been created before calling run.
	//
	if (IsListEmpty(&ReadyQueue)) {
		return;
	}

	//
	// Switch to a user thread.
	//
	MainThread = &Thread;
	RunningThread = MainThread;
	Schedule();

	//
	// When we get here, there are no more runnable user threads.
	//
	_ASSERTE(IsListEmpty(&ReadyQueue));
	_ASSERTE(NumberOfThreads == 0);

	//
	// Allow another call to UtRun().
	//
	RunningThread = NULL;
	MainThread = NULL;
}

//
// Terminates the execution of the currently running thread. All associated
// resources are released after the context switch to the next ready thread.
//
VOID UtExit() {
	NumberOfThreads -= 1;						// Decrement the threads counter
	RunningThread->State = Finished;			// Change the thread's State to 'Finished'
	EventSet(&RunningThread->finishedEvt);		// Set the Event - The thread is finishing now!
	InternalExit(RunningThread, ExtractNextReadyThread());
	_ASSERTE(!"Supposed to be here!");
}

//
// Relinquishes the processor to the first user thread in the ready queue.
// If there are no ready threads, the function returns immediately.
//
VOID UtYield() {
	if (!IsListEmpty(&ReadyQueue)) {
		RunningThread->State = Ready; // We are going to change the running thread, so the current running thread needs to have its state updated to "Ready"
		InsertTailList(&ReadyQueue, &RunningThread->Link);
		Schedule();
	}
}

//
// Returns a HANDLE to the executing user thread.
//
HANDLE UtSelf() {
	return (HANDLE)RunningThread;
}

//
// Halts the execution of the current user thread.
//
VOID UtDeactivate() {
	RunningThread->State = Blocked;
	Schedule();
}


//
// Exercicio 1 - Serie 2 - Parte A - Catia Ormonde, nº36923
// This function does not return until the thread (represented by the HANDLE passed as argument) is terminated.
// If that thread has already called UtExit(), the function returns -1. Otherwise, in case of success, the function returns 0.
//
// My solution is: I updated the UThreadInternal-> added 'State' (represents the state of the thread), flag 'IsWaitingForCompletion' and the EVENT 'finishedEvt'
//
// The Idea: 
// First, calling "UtJoin": If the thread is Ready (this is checked by verifying the thread's 'State') and hasn't finished before yet, 
// its "IsWaitingForCompletion" flag is set to TRUE and the thread is Deactivated.
//
// When the scheduler is getting another thread ready to run, it checks if that thread "IsWaitingForCompletion" - If so, it Activates that thread.
// Once activated, the thread will be waiting for the "finishedEvt" event to be set. 
//
// The "finishedEvt" will be set in "UtExit". Once Set, the thread will return 0 (success).
//
int UtJoin(HANDLE thread){

	PUTHREAD uthread;

	// If the thread handle is invalid, the thread has already finished, or there are no more threads return -1

	if (thread == NULL || NumberOfThreads == 0)
		return -1;

	uthread = (PUTHREAD)thread;

	if (uthread->State == Ready){

		// If the thread is Ready and it has never called UtJoin before (it isn't waiting yet)
		if (!uthread->IsWaitingForCompletion){

			// Activate the 'IsWaitingForCompletion' flag and Deactivate the thread - Another function will activate it later, when it's time for it to run.
			uthread->IsWaitingForCompletion = TRUE;
			UtDeactivate();

			if (!uthread->IsWaitingForCompletion){	// After being activated again: If the thread really finished, return 0 (success)
				EventWait(&uthread->finishedEvt);
				return 0;
			}
		}
	}
	return -1; // If the thread isn't ready return -1
}

//
// Exercicio 2 - Serie 2 - Parte A - Catia Ormonde, nº36923
// Blocks the thread that invokes this function for, at least, "sleepTimeInMillis" milliseconds
// This function needs another auxiliary function, to Activate the thread that is deactivated here!
//

typedef struct SleepingThread{
	DWORD extraTimeSleeping;
	HANDLE uthread;
	LIST_ENTRY Link;
} SLEEPING_UTHREAD, *PSLEEPING_UTHREAD;

DWORD UtSleep(DWORD sleepTimeInMillis){

	DWORD initialTime = GetTickCount();		// Start counting the time that the thread will be sleeping
	HANDLE runningThread = UtSelf();
	PSLEEPING_UTHREAD sleepingThread = ((PSLEEPING_UTHREAD)malloc(sizeof (struct SleepingThread)));// Will be updated with the sleep time (counted outside this function)

	sleepingThread->extraTimeSleeping = 0;				// Initialize the extra sleeping time = 0
	sleepingThread->uthread = UtSelf();					// Store the ref to the running thread - so that it can be activated later
	InsertTailList(&SleepyQueue, &(sleepingThread->Link));	// This thread's handle is now saved, to be used in "UtSleepHelper" - a function that will activate this thread whenever needed

	for (;;){
		DWORD timePassedInThisFunction = GetTickCount() - initialTime;							// Update the sleeping time (in this function)
		if (sleepTimeInMillis > timePassedInThisFunction + sleepingThread->extraTimeSleeping){	// If the minimum time has not gone by yet
			InsertHeadList(&SleepyQueue, &(sleepingThread->Link));								// Add the sleepy thread to the list again, this time to its head
			UtDeactivate();																		// Deactivate this running thread
		}
		else {
			return timePassedInThisFunction + (sleepingThread->extraTimeSleeping);
		}
	}
	free(sleepingThread);
}

VOID UtSleepHelper(){
	DWORD initialTime = GetTickCount();
	if (!IsListEmpty(&SleepyQueue)){											// If there are any threads sleeping (in the SleepyQueue)
		PSLEEPING_UTHREAD sleepingThread = CONTAINING_RECORD(RemoveHeadList(&SleepyQueue), SLEEPING_UTHREAD, Link);	// Get the first in line (Queue is FIFO)
		sleepingThread->extraTimeSleeping += (GetTickCount() - initialTime);	// Increment its "extraTimeSleeping" counter
		UtActivate(sleepingThread->uthread);									// Activate the corresponding UThread
	}
}

//
// Places the specified user thread at the end of the ready queue, where it
// becomes eligible to run.
//
VOID UtActivate(HANDLE ThreadHandle) {
	((PUTHREAD)ThreadHandle)->State = Ready;
	InsertTailList(&ReadyQueue, &((PUTHREAD)ThreadHandle)->Link);
}

///////////////////////////////////////
//
// Definition of internal operations.
//

//
// The trampoline function that a user thread begins by executing, through
// which the associated function is called.
//
VOID InternalStart() {
	RunningThread->Function(RunningThread->Argument);
	UtExit();
}

//
// functions with implementation dependent of X86 or x64 platform
//

#ifndef UTHREAD_X64
//
// Creates a user thread to run the specified function. The thread is placed
// at the end of the ready queue.
//
HANDLE UtCreate(UT_FUNCTION Function, UT_ARGUMENT Argument) {
	PUTHREAD Thread;

	//
	// Dynamically allocate an instance of UTHREAD and the associated stack.
	//
	Thread = (PUTHREAD)malloc(sizeof (UTHREAD));
	Thread->Stack = (PUCHAR)malloc(STACK_SIZE);
	_ASSERTE(Thread != NULL && Thread->Stack != NULL);

	//
	// Zero the stack for emotional confort.
	//
	memset(Thread->Stack, 0, STACK_SIZE);

	//
	// Memorize Function and Argument for use in InternalStart.
	//
	Thread->Function = Function;
	Thread->Argument = Argument;

	//
	// Map an UTHREAD_CONTEXT instance on the thread's stack.
	// We'll use it to save the initial context of the thread.
	//
	// +------------+
	// | 0x00000000 |    <- Highest word of a thread's stack space
	// +============+       (needs to be set to 0 for Visual Studio to
	// |  RetAddr   | \     correctly present a thread's call stack).
	// +------------+  |
	// |    EBP     |  |
	// +------------+  |
	// |    EBX     |   >   Thread->ThreadContext mapped on the stack.
	// +------------+  |
	// |    ESI     |  |
	// +------------+  |
	// |    EDI     | /  <- The stack pointer will be set to this address
	// +============+       at the next context switch to this thread.
	// |            | \
		// +------------+  |
	// |     :      |  |
	//       :          >   Remaining stack space.
	// |     :      |  |
	// +------------+  |
	// |            | /  <- Lowest word of a thread's stack space
	// +------------+       (Thread->Stack always points to this location).
	//

	Thread->ThreadContext = (PUTHREAD_CONTEXT)(Thread->Stack +
		STACK_SIZE - sizeof (ULONG)-sizeof (UTHREAD_CONTEXT));

	//
	// Set the thread's initial context by initializing the values of EDI,
	// EBX, ESI and EBP (must be zero for Visual Studio to correctly present
	// a thread's call stack) and by hooking the return address.
	// 
	// Upon the first context switch to this thread, after popping the dummy
	// values of the "saved" registers, a ret instruction will place the
	// address of InternalStart on EIP.
	//
	Thread->ThreadContext->EDI = 0x33333333;
	Thread->ThreadContext->EBX = 0x11111111;
	Thread->ThreadContext->ESI = 0x22222222;
	Thread->ThreadContext->EBP = 0x00000000;
	Thread->ThreadContext->RetAddr = InternalStart;

	//
	// Ready the thread.
	//
	NumberOfThreads += 1;
	UtActivate((HANDLE)Thread);

	return (HANDLE)Thread;
}

//
// Performs a context switch from CurrentThread to NextThread.
// __fastcall sets the calling convention such that CurrentThread is in ECX and NextThread in EDX.
// __declspec(naked) directs the compiler to omit any prologue or epilogue.
//
__declspec(naked)
VOID __fastcall ContextSwitch(PUTHREAD CurrentThread, PUTHREAD NextThread) {
	__asm {
		// Switch out the running CurrentThread, saving the execution context on the thread's own stack.   
		// The return address is atop the stack, having been placed there by the call to this function.
		//
		push	ebp
			push	ebx
			push	esi
			push	edi
			//
			// Save ESP in CurrentThread->ThreadContext.
			//
			mov		dword ptr[ecx].ThreadContext, esp
			//
			// Set NextThread as the running thread.
			//
			mov     RunningThread, edx
			//
			// Load NextThread's context, starting by switching to its stack, where the registers are saved.
			//
			mov		esp, dword ptr[edx].ThreadContext
			pop		edi
			pop		esi
			pop		ebx
			pop		ebp
			//
			// Jump to the return address saved on NextThread's stack when the function was called.
			//
			ret
	}
}

//
// Frees the resources associated with Thread.
// __fastcall sets the calling convention such that Thread is in ECX.
//

static
VOID __fastcall CleanupThread(PUTHREAD Thread) {
	free(Thread->Stack);
	free(Thread);
}

//
// Frees the resources associated with CurrentThread and switches to NextThread.
// __fastcall sets the calling convention such that CurrentThread is in ECX and NextThread in EDX.
// __declspec(naked) directs the compiler to omit any prologue or epilogue.
//
__declspec(naked)
VOID __fastcall InternalExit(PUTHREAD CurrentThread, PUTHREAD NextThread) {
	__asm {

		//
		// Set NextThread as the running thread.
		//
		mov     RunningThread, edx

			//
			// Load NextThread's stack pointer before calling CleanupThread(): making the call while
			// using CurrentThread's stack would mean using the same memory being freed -- the stack.
			//
			mov		esp, dword ptr[edx].ThreadContext

			call    CleanupThread

			//
			// Finish switching in NextThread.
			//
			pop		edi
			pop		esi
			pop		ebx
			pop		ebp
			ret
	}
}

#else

//
// Creates a user thread to run the specified function. The thread is placed
// at the end of the ready queue.
//
HANDLE UtCreate(UT_FUNCTION Function, UT_ARGUMENT Argument) {
	PUTHREAD Thread;

	//
	// Dynamically allocate an instance of UTHREAD and the associated stack.
	//
	Thread = (PUTHREAD)malloc(sizeof (UTHREAD));
	Thread->Stack = (PUCHAR)malloc(STACK_SIZE);
	_ASSERTE(Thread != NULL && Thread->Stack != NULL);

	//
	// Zero the stack for emotional confort.
	//
	memset(Thread->Stack, 0, STACK_SIZE);

	//
	// Memorize Function and Argument for use in InternalStart.
	//
	Thread->Function = Function;
	Thread->Argument = Argument;

	//
	// Map an UTHREAD_CONTEXT instance on the thread's stack.
	// We'll use it to save the initial context of the thread.
	//
	// +------------+  <- Highest word of a thread's stack space
	// | 0x00000000 |    (needs to be set to 0 for Visual Studio to
	// +------------+      correctly present a thread's call stack).   
	// | 0x00000000 |  \
		// +------------+   |
	// | 0x00000000 |   | <-- Shadow Area for Internal Start 
	// +------------+   |
	// | 0x00000000 |   |
	// +------------+   |
	// | 0x00000000 |  /
	// +============+       
	// |  RetAddr   | \    
	// +------------+  |
	// |    RBP     |  |
	// +------------+  |
	// |    RBX     |   >   Thread->ThreadContext mapped on the stack.
	// +------------+  |
	// |    RDI     |  |
	// +------------+  |
	// |    RSI     |  |
	// +------------+  |
	// |    R12     |  |
	// +------------+  |
	// |    R13     |  |
	// +------------+  |
	// |    R14     |  |
	// +------------+  |
	// |    R15     | /  <- The stack pointer will be set to this address
	// +============+       at the next context switch to this thread.
	// |            | \
		// +------------+  |
	// |     :      |  |
	//       :          >   Remaining stack space.
	// |     :      |  |
	// +------------+  |
	// |            | /  <- Lowest word of a thread's stack space
	// +------------+       (Thread->Stack always points to this location).
	//

	Thread->ThreadContext = (PUTHREAD_CONTEXT)(Thread->Stack +
		STACK_SIZE - sizeof (UTHREAD_CONTEXT)-sizeof(ULONGLONG)* 5);

	//
	// Set the thread's initial context by initializing the values of 
	// registers that must be saved by the called (R15,R14,R13,R12, RSI, RDI, RBCX, RBP)

	// 
	// Upon the first context switch to this thread, after popping the dummy
	// values of the "saved" registers, a ret instruction will place the
	// address of InternalStart on EIP.
	//
	Thread->ThreadContext->R15 = 0x77777777;
	Thread->ThreadContext->R14 = 0x66666666;
	Thread->ThreadContext->R13 = 0x55555555;
	Thread->ThreadContext->R12 = 0x44444444;
	Thread->ThreadContext->RSI = 0x33333333;
	Thread->ThreadContext->RDI = 0x11111111;
	Thread->ThreadContext->RBX = 0x22222222;
	Thread->ThreadContext->RBP = 0x11111111;
	Thread->ThreadContext->RetAddr = InternalStart;

	//
	// Ready the thread.
	//
	NumberOfThreads += 1;
	UtActivate((HANDLE)Thread);

	return (HANDLE)Thread;
}


//
// Frees the resources associated with Thread.
// In X64 calling convention Thread comes in RCX.
//
VOID CleanupThread(PUTHREAD Thread) {
	free(Thread->Stack);
	free(Thread);
}


#endif



/***
SERIE 2 - PARTE A - TESTS
***/

// auxFunction_for_Ex1 (This function is USELESS. Just using it for tests!)
DWORD auxFunction_for_Ex1(DWORD countUpTo){ // counts up to the arg given; and returns the total time
	DWORD initTime = GetTickCount();
	DWORD c = 0;
	for (;;){
		if (c < countUpTo)
			c++;
		else break;
	}
	return GetTickCount() - initTime;
}

void EX1_TEST() {


	//TODO - Test Ex.1 = UtJoin(HANDLE thread)

	// Create the UThread
	HANDLE h1 = UtCreate(auxFunction_for_Ex1, (VOID *)10000); // Aux function = counts up to 10000 in a For Loop 
	HANDLE h2 = UtCreate(auxFunction_for_Ex1, (VOID *)10000); 
	HANDLE h3 = UtCreate(auxFunction_for_Ex1, (VOID *)10000); 

	UtJoin(h1);
	UtJoin(h2);
	UtJoin(h3);

	// Schedule the created UThreads to run!
	Schedule();
}

void auxFunction_for_Ex2(){
	UtCreate(UtSleep, (VOID *)5000); // Wait 5 sec
}

void EX2_TEST() {

	//TODO - Test Ex.2 = UtSleep(DWORD milis) & UtSleepHelper
	UtCreate(auxFunction_for_Ex2, (VOID*)NULL);
	//UtCreate(UtSleepHelper, (VOID *)NULL);  // Doesn't receive args

	Schedule();
}

void EX3_TEST() {
}

int _tmain(int argc, TCHAR argv[]) {

	UTHREAD mainThread;

	EX1_TEST();
	EX2_TEST();
	EX3_TEST();

	MainThread = &mainThread;
	RunningThread = &mainThread;
	Schedule();

	_tprintf(_T("Terminating main system thread\n"));
	return 0;
}
