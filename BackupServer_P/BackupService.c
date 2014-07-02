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

	backupService.repoPath = repoPath;
	backupService.nRequests = 0;
	backupService.putRequest = 0;
	backupService.getRequest = 0;
	backupService.isAlive = TRUE;
	backupService.processID = GetCurrentProcessId();

	backupService.rqstsMutex = CreateMutex(NULL, FALSE, NULL);		// Mutex sem nome que trata de lidar com a lista de requestss
	backupService.hasWork = CreateEvent(NULL, FALSE, FALSE, NULL);	// Auto-Reset Event (FALSE), sem nome (NULL)
	backupService.isFull = CreateEvent(NULL, FALSE, FALSE, NULL);	// Auto-Reset Event (FALSE), sem nome (NULL)

	printf("\n++++ Server with Service: %s is online! ++++\n\n", serviceName);
	printf("\n++++ Repository Path: %s ++++\n\n", repoPath);

	return backupService;
}

BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor){
	if (service.isAlive == FALSE){
		printf("\n++++ Server is no longer working! ++++\n\n ");
		return FALSE;
	}

	if (service.nRequests == 0){
		printf("\n++++ Server has nothing to do and it will sleep until there's work! ++++\n\n ");
		WaitForSingleObject(service.hasWork, INFINITE);
	}

	// If the server is online and has work to do, then it should process the next entry on requests
	WaitForSingleObject(service.rqstsMutex, INFINITE);
	{
		BACKUPENTRY backupEntry = service.requests[service.getRequest++];

		switch (backupEntry.operation){
			case Backup:
				if (!CopyFile(
					backupEntry.clientFolder,	// _In_  LPCTSTR lpExistingFileName -> If lpExistingFileName does not exist, CopyFile fails, and GetLastError returns ERROR_FILE_NOT_FOUND.
					service.repoPath,			// _In_  LPCTSTR lpNewFileName
					FALSE						// _In_  BOOL bFailIfExists -> If this parameter is TRUE and the new file specified by lpNewFileName already exists, the function fails. If this parameter is FALSE and the new file already exists, the function overwrites the existing file and succeeds.
					))
				{
					printf("Couldn't copy the file. Error %s", GetLastError());
					SetEvent(backupEntry.hArray[1]); // and return TRUE; ?
				}
				else{
					SetEvent(backupEntry.hArray[0]); // and return TRUE; ?
				}
				break;
			case Restore:
				if (!CopyFile(
					service.repoPath,			// _In_  LPCTSTR lpExistingFileName -> If lpExistingFileName does not exist, CopyFile fails, and GetLastError returns ERROR_FILE_NOT_FOUND.
					backupEntry.clientFolder,	// _In_  LPCTSTR lpNewFileName
					FALSE						// _In_  BOOL bFailIfExists -> If this parameter is TRUE and the new file specified by lpNewFileName already exists, the function fails. If this parameter is FALSE and the new file already exists, the function overwrites the existing file and succeeds.
					))
				{
					printf("Couldn't copy the file. Error %s", GetLastError());
					SetEvent(backupEntry.hArray[1]); // and return FALSE; ?
				}
				else{
					SetEvent(backupEntry.hArray[0]); // and return TRUE; ?
				}
				break;
			case Exit:
				// TODO
				break;
			default:
				printf("An error has occurred: %s", GetLastError());
				return FALSE;
		}

		ResetEvent(service.isFull); // If a request was replied, then the server has at least one slot available
	}
	ReleaseMutex(service.rqstsMutex);

	return TRUE;
}

BOOL CloseBackupService(HBACKUPSERVICE service){
	return FALSE;
}

/****************************** Client *********************************/
HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){
	HBACKUPSERVICE backupService;
	LPTSTR pBuf;

	backupService = *((PHBACKUPSERVICE)malloc(sizeof(HBACKUPSERVICE)));

	/*
	Os clientes, que não poderão ser mais do que MAX_CLIENTS em simultâneo, encontram o espaço de memória
	partilhada pelo nome atribuído. O espaço de memória partilhada contém um array de MAX_CLIENTS posições,
	em que cada entrada contém uma instância de PBACKUPENTRY e os elementos necessários para a sincronização 
	entre o servidor e um cliente específico.
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

	backupService = *(PHBACKUPSERVICE)pBuf;

	printf("\n++++ Found the Server with service name: %s ++++\n", serviceName);

	return backupService;
}

BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file){
	HANDLE rqstsMutexDup, hasWorkDup, isFullDup;
	BACKUPENTRY backupEntry;
	DWORD wfmoRet;

	if (service.isAlive == TRUE){
		printf("\n+++++ Server is offline, declining this request! +++++\n");
		return FALSE;
	}


	// Is it better to put WaitForSingleObject(rqstsMutexDup, INFINITE) here?
	
	if (service.nRequests == MAX_CLIENTS){
		printf("\n+++++ Too many clients, sorry :/ Wait a bit... +++++\n");
		WaitForSingleObject(service.isFull, INFINITE);
	}

	if (!DuplicateHandle(
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, service.processID),
		service.rqstsMutex,
		GetCurrentProcess(),
		&rqstsMutexDup,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS))
	{
		printf("1. Could not create duplicate the Request Mutex Handle (%d).\n"), GetLastError();
		return FALSE;
	}
	
	if (!DuplicateHandle(
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, service.processID),
		service.hasWork,
		GetCurrentProcess(),
		&hasWorkDup,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS))
	{
		printf("2. Could not create duplicate the hasWork Mutex Handle (%d).\n"), GetLastError();
		return FALSE;
	}

	// I need to do this here because of error handling
	if (!DuplicateHandle(
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, service.processID),
		service.isFull,
		GetCurrentProcess(),
		&isFullDup,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS))
	{
		printf("3. Could not create duplicate the isFull Mutex Handle (%d).\n"), GetLastError();
		return FALSE;
	}

	WaitForSingleObject(rqstsMutexDup, INFINITE);
	{
		//printf("[C] Acquire Mutex\n");
		backupEntry = service.requests[service.putRequest++];

		backupEntry.clientProcID = GetCurrentProcessId();
		backupEntry.operation = Backup;
		backupEntry.file = file;

		backupEntry.hArray[0] = CreateEvent(NULL, FALSE, FALSE, "success");	// Auto-Reset Event (FALSE), com nome ("success")
		backupEntry.hArray[1] = CreateEvent(NULL, FALSE, FALSE, "failure");	// Auto-Reset Event (FALSE), com nome ("failure")

		SetEvent(hasWorkDup);
	}
	ReleaseMutex(rqstsMutexDup);

	wfmoRet = WaitForMultipleObjects(
		2,						// number of objects in array
		&backupEntry.hArray,	// array of objects
		FALSE,					// wait for any object
		INFINITE);				// Infinite wait

	switch (wfmoRet)
	{
		// hArray[0] was signaled
		case WAIT_OBJECT_0 + 0:
			// TODO: Perform tasks required by this event
			printf("Operation returned success.\n");
			break;

		// hArray[1] was signaled
		case WAIT_OBJECT_0 + 1:
			printf("Operation returned failure.\n");
			break;

		case WAIT_TIMEOUT:
			printf("Wait timed out.\n");
			break;

		// Return value is invalid.
		default:
			printf("Wait error: %d\n", GetLastError());
			ExitProcess(0);
	}

	return TRUE;
}

BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file){
	HANDLE rqstsMutexDup, hasWorkDup, isFullDup;
	BACKUPENTRY backupEntry;
	DWORD wfmoRet;

	if (service.isAlive == TRUE){
		printf("\n+++++ Server is offline, declining this request! +++++\n");
		return FALSE;
	}

	// Is it better to put WaitForSingleObject(rqstsMutexDup, INFINITE) here?

	if (service.nRequests == MAX_CLIENTS){
		printf("\n+++++ Too many clients, sorry :/ Wait a bit... +++++\n");
		WaitForSingleObject(service.isFull, INFINITE);
	}

	if (!DuplicateHandle(
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, service.processID),
		service.rqstsMutex,
		GetCurrentProcess(),
		&rqstsMutexDup,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS))
	{
		printf("1. Could not create duplicate the Request Mutex Handle (%d).\n"), GetLastError();
		return FALSE;
	}

	if (!DuplicateHandle(
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, service.processID),
		service.hasWork,
		GetCurrentProcess(),
		&hasWorkDup,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS))
	{
		printf("2. Could not create duplicate the hasWork Mutex Handle (%d).\n"), GetLastError();
		return FALSE;
	}

	// I need to do this here because of error handling
	if (!DuplicateHandle(
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, service.processID),
		service.isFull,
		GetCurrentProcess(),
		&isFullDup,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS))
	{
		printf("3. Could not create duplicate the isFull Mutex Handle (%d).\n"), GetLastError();
		return FALSE;
	}

	WaitForSingleObject(rqstsMutexDup, INFINITE);
	{
		//printf("[C] Acquire Mutex\n");
		backupEntry = service.requests[service.putRequest++];

		backupEntry.clientProcID = GetCurrentProcessId();
		backupEntry.operation = Restore;
		backupEntry.file = file;

		backupEntry.hArray[0] = CreateEvent(NULL, FALSE, FALSE, "success");	// Auto-Reset Event (FALSE), com nome ("success")
		backupEntry.hArray[1] = CreateEvent(NULL, FALSE, FALSE, "failure");	// Auto-Reset Event (FALSE), com nome ("failure")

		SetEvent(hasWorkDup);

		if (service.putRequest == MAX_CLIENTS)
			SetEvent(isFullDup);

	}
	ReleaseMutex(rqstsMutexDup);

	wfmoRet = WaitForMultipleObjects(
		2,						// number of objects in array
		&backupEntry.hArray,	// array of objects
		FALSE,					// wait for any object
		INFINITE);				// Infinite wait

	switch (wfmoRet)
	{
		// hArray[0] was signaled
		case WAIT_OBJECT_0 + 0:
			// TODO: Perform tasks required by this event
			printf("Operation returned success.\n");
			break;

		// hArray[1] was signaled
		case WAIT_OBJECT_0 + 1:
			printf("Operation returned failure.\n");
			break;

		case WAIT_TIMEOUT:
			printf("Wait timed out.\n");
			break;

			// Return value is invalid.
		default:
			printf("Wait error: %d\n", GetLastError());
			ExitProcess(0);
	}

	return TRUE;
}

BOOL StopBackupService(TCHAR * serviceName){
	return FALSE;
}