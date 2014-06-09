#include <stdio.h>
#include "CounterUtils.h"

#define MILLISECONDS 1000
#define MICROSECONDS 1000000

LARGE_INTEGER StartCounter(){
	LARGE_INTEGER startTime;

	if (!QueryPerformanceCounter(&startTime))
		printf("QueryPerformanceCounter failed!\n");

	return startTime;
}

LONGLONG GetCounter(LARGE_INTEGER startTime){
	LARGE_INTEGER endTime, elapsedTime, pcFreq;

	if (!QueryPerformanceFrequency(&pcFreq))
		printf("QueryPerformanceFrequency failed!\n");

	QueryPerformanceCounter(&endTime);
	elapsedTime.QuadPart = endTime.QuadPart - startTime.QuadPart;

	elapsedTime.QuadPart *= MICROSECONDS;
	elapsedTime.QuadPart /= pcFreq.QuadPart;

	return elapsedTime.QuadPart;
}