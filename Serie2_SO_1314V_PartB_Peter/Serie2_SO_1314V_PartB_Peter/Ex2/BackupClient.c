#include "BackupClient.h"

HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){
	HBACKUPSERVICE backupService;
	LPTSTR pBuf;
	DWORD clientId;

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
		printf("1. Could not create file mapping object (%d).\n"), GetLastError();
		return;
	}

	pBuf = (LPTSTR)MapViewOfFile(backupService.hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(backupService));

	if (pBuf == NULL)
	{
		printf("2. Could not create file mapping object (%d).\n"), GetLastError();
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

BOOL CloseBackupService(HBACKUPSERVICE service){
	return FALSE;
}