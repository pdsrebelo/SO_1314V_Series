#include "BackupService.h"

/****************************** Server *********************************/
HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath){
	LPCTSTR pBuf;

	HBACKUPSERVICE backupService = *(PHBACKUPSERVICE)malloc(sizeof(HBACKUPSERVICE));

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
		printf("Could not create file mapping object (%d).\n", GetLastError());
		return;
	}

	pBuf = (LPCTSTR)MapViewOfFile(
		backupService.hMapFile,			//_In_  HANDLE	hFileMappingObject,
		FILE_MAP_ALL_ACCESS,			//_In_  DWORD	dwDesiredAccess,
		0,								//_In_  DWORD	dwFileOffsetHigh,
		0,								//_In_  DWORD	dwFileOffsetLow,
		sizeof(backupService)			//_In_  SIZE_T	dwNumberOfBytesToMap
		);

	if (pBuf == NULL){
		printf("Could not create file mapping object (%d).\n", GetLastError());
		CloseHandle(backupService.hMapFile);
		return;
	}

	backupService = *(PHBACKUPSERVICE)pBuf;

	printf("\n++++ Server with Service: %s is online! ++++\n\n", serviceName);

	return backupService;
}

BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor){
	return FALSE;
}

BOOL CloseBackupService(HBACKUPSERVICE service){
	return FALSE;
}

/****************************** Client *********************************/
HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){
	HBACKUPSERVICE backupService;
	LPTSTR pBuf;

	backupService = *((PHBACKUPSERVICE)malloc(sizeof(HBACKUPSERVICE)));
	//reqSession.reqService = (PHBACKUPSERVICE)malloc(sizeof(HBACKUPSERVICE));

	/*
	Os clientes, que não poderão ser mais do que MAX_CLIENTS em simultâneo, encontram o espaço de memória
	partilhada pelo nome atribuído. O espaço de memória partilhada contém um array de MAX_CLIENTS posições,
	em que cada entrada contém uma instância de REQ e os elementos necessários para a sincronização entre o
	servidor e um cliente específico. Cada entrada desse array é do tipo SLOT.
	*/

	backupService.hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		serviceName);

	if (backupService.hMapFile == NULL){
		printf("1. Could not create file mapping object (%d).\n", GetLastError());
		return;
	}

	pBuf = (LPTSTR)MapViewOfFile(backupService.hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(backupService));

	if (pBuf == NULL)
	{
		printf("2. Could not create file mapping object (%d).\n", GetLastError());
		CloseHandle(backupService.hMapFile);
		return;
	}

	//backupService->reqService = *(PHBACKUPSERVICE)pBuf;

	return backupService;
}

BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file){
	return FALSE;
}

BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file){
	return FALSE;
}

BOOL StopBackupService(TCHAR * serviceName){
	return FALSE;
}