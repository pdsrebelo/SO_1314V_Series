#include "BackupServer.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath){
	LPCTSTR pBuf;

	HBACKUPSERVICE backupService = *(PHBACKUPSERVICE) malloc(sizeof(HBACKUPSERVICE));

	/* Um processo (servidor) instancia o serviço (CreateBackupService), ficando responsável 
	por realizar as cópias dos ficheiros à medida que os pedidos vão aparecendo (ProcessNextEntry) */

	backupService.hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(backupService),
		serviceName
		);

	if (backupService.hMapFile == NULL){
		printf("Could not create file mapping object (%d).\n"), GetLastError();
		return;
	}

	pBuf = (LPCTSTR)MapViewOfFile(
		backupService.hMapFile,		//_In_  HANDLE hFileMappingObject,
		FILE_MAP_ALL_ACCESS,			//_In_  DWORD dwDesiredAccess,
		0,								//_In_  DWORD dwFileOffsetHigh,
		0,								//_In_  DWORD dwFileOffsetLow,
		sizeof(backupService)			//_In_  SIZE_T dwNumberOfBytesToMap
		);

	if (pBuf == NULL){
		printf("Could not create file mapping object (%d).\n"), GetLastError();
		CloseHandle(backupService.hMapFile);
		return;
	}

	backupService = *(PHBACKUPSERVICE) pBuf;

	printf("\n++++ Server with Service: %s is online! ++++\n\n", serviceName);

	return backupService;
}

BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor){
	return FALSE;
}

BOOL CloseBackupService(HBACKUPSERVICE service){
	return FALSE;
}