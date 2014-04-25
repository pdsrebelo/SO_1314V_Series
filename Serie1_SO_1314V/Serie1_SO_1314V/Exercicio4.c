#define _UNICODE
#define UNICODE

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define DATALEN 1024*1024*256U

static BYTE data[DATALEN] = { 1, 2 };

ULONG ReadData()
{
	ULONG sum = 0;
	ULONG i;

	for (i = 0; i < DATALEN; ++i)
		sum += data[i];

	return sum;
}

VOID WriteData(int v)
{
	ULONG i;

	for (i = 0; i < DATALEN; ++i)
		data[i] = v;
}

VOID PressToContinue(LPCTSTR phaseName)
{
	_tprintf(_T("Prima RETURN para %s\n"), phaseName);
	getchar();
}

int _tmain2(int argc, _TCHAR* argv[])
{
	_tprintf(_T("ProcessId = %d\n"), GetCurrentProcessId());
	PressToContinue(_T("iniciar"));

	ReadData();
	PressToContinue(_T("continuar"));

	WriteData(1);
	PressToContinue(_T("terminar"));

	return 0;
}
