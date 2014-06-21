#include "stdafx.h"
#include <iostream>
#include <tchar.h>
#include "BackupServerDll.h"
#include <cstdio>
#include <wchar.h>

HBACKUPSERVICE backupService;

// Instanciar o serviço
HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath){
	SIZE_T maxSize = sizeof(BACKUPSERVICE);
	DWORD i = 0;
	HANDLE hfMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, maxSize, serviceName);

	if (hfMap == NULL){
		printf("\nERRO: Nao foi possivel chamar CreateFileMapping! #%d", GetLastError());
		return NULL;
	}

	backupService = (HBACKUPSERVICE)MapViewOfFile(hfMap, FILE_MAP_WRITE|FILE_MAP_READ, 0, 0, maxSize);
	if (backupService == NULL){
		printf("\nERRO: Nao foi possivel mapear em memoria! #%d",GetLastError());
		return NULL;
	}
	
	wcsncpy_s(backupService->fileStoragePath, repoPath, wcslen(repoPath)+1);
	backupService->nRequests = 0;
	wcsncpy_s(backupService->serviceName, serviceName, wcslen(serviceName)+1);
	backupService->hServiceExclusion = CreateMutex(NULL, FALSE, (LPCWSTR)"serverMutex"); // null para usar os security attributes por omissão
	backupService->serverProcess = GetCurrentProcess();

	if (backupService->hServiceExclusion == NULL){
		printf("\nERRO: Nao foi possivel criar o mutex! #%d", GetLastError());
		return NULL;
	}
	return backupService;
}

// Manda realizar a operacao escolhida, sobre o ficheiro
BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor){
	HBACKUPENTRY pRequest;
	HANDLE hMutexDup;
	DWORD nRequests, reqIdx;
	BOOL requestSuccess = FALSE; 

	// Must duplicate the Mutex handle here
	if (DuplicateHandle(service->serverProcess,
		service->hServiceExclusion, GetCurrentProcess(), &hMutexDup, 0, FALSE, DUPLICATE_SAME_ACCESS) == NULL){
		printf("\nERRO: Nao foi possivel duplicar o handle para o mutex! (%d)", GetLastError());
		return FALSE;
	}

	WaitForSingleObject(hMutexDup, INFINITE);//WaitForSingleObject(service->hServiceExclusion, INFINITE);
	{
		nRequests = service->nRequests;
		// Ir buscar um pedido, para mandar processar
		if (nRequests > 0){
			reqIdx = nRequests-1;	// Ir buscar a entrada correspondente ao ultimo pedido
			pRequest = &service->requests[reqIdx];
			requestSuccess = processor(pRequest);
			// Se houve sucesso no processamento do pedido, decrementar nº de pedidos
			if (requestSuccess){
				service->nRequests --;
				printf("\nSucesso: Pedido processado!");
			}
			else{
				printf("\nERRO: O pedido nao foi processado correctamente!");
			}
		}
	}
	ReleaseMutex(hMutexDup);//ReleaseMutex(service->hServiceExclusion);
	CloseHandle(hMutexDup);
	return requestSuccess;
}

// Faz a reposicao (Restore) ou guarda uma copia do ficheiro (Backup), conforme o bool recebido.
// backupNrestore = true -> Backup
// backupNrestore = false -> Restore
BOOL CopyFile(BOOL backupNrestore, HBACKUPENTRY copyRequest){
	
	FILE * origin; FILE * destiny;
	CHAR auxBuffer[MAX_PATH];
	DWORD i, ofnIdx; //ofnIdx serve para copiar todos os chars para um char* local, com o nome do ficheiro original
	CHAR* separator = "\\", *terminator = "\0";
	SIZE_T sz, filePathSize = strlen((CHAR*)backupService->fileStoragePath) + strlen(separator) + strlen((CHAR*)copyRequest->file) + 1;
	CHAR*repoFileName, *originalFileName;
	
	repoFileName = (CHAR*)malloc(filePathSize);
	originalFileName = (CHAR*)malloc(strlen((CHAR*)copyRequest->file) + 1);

	sz = strlen((CHAR*)backupService->fileStoragePath) + 1;
	strcpy_s(repoFileName, sz, (CHAR*)backupService->fileStoragePath);

	sz += strlen(separator);
	strcat_s(repoFileName, filePathSize, separator);

	ofnIdx = 0;
	for (i = 0; i < STR_MAX_SIZE; i++){
		CHAR * part = (CHAR*)(copyRequest->file + i*sizeof(CHAR));
		if (strcmp(part, terminator) == 0)
			break;
		sz++; ofnIdx++;
		strcat_s(repoFileName, sz, part);

		if (ofnIdx == 1)
			strcpy_s(originalFileName, ofnIdx + 1, part);
		else
			strcat_s(originalFileName, ofnIdx + 1, part);
	}

	if (fopen_s(&origin, backupNrestore?originalFileName : repoFileName, "rb") != 0){// abrir ficheiro origem para ler
		printf("\nERRO... O ficheiro %s nao existe!", backupNrestore ? originalFileName : repoFileName);
		return FALSE;
	}

	if (fopen_s(&destiny, backupNrestore ? repoFileName : originalFileName, "wb+") != 0){
		printf("\nERRO... Nao foi possivel criar o ficheiro %s ! Verifique se o caminho existe!", backupNrestore ? repoFileName : originalFileName);
		return FALSE;
	}

	while (fgets(auxBuffer, sizeof(auxBuffer), origin) != NULL){
		fprintf_s(destiny, auxBuffer);
	}

	fclose(origin);
	fclose(destiny);
	return TRUE;
}

// Repoe o ficheiro original (copia o ficheiro novamente, mas na ordem inversa)
BOOL RestoreFileFunction(HBACKUPENTRY pentry){
	return CopyFile(FALSE, pentry);
}

//Copia ficheiro
BOOL BackupFileFunction(HBACKUPENTRY pentry){
	return CopyFile(TRUE, pentry);
}

BOOL SendNewRequest(HBACKUPSERVICE service, DWORD clientProcId, BACKUP_OPERATION operation, TCHAR * file){
	DWORD nReq;
	HANDLE hMutexDup;

	if (file==NULL){
		printf("\nERROR: No file was specified!");
		return FALSE;
	}

	// Must duplicate the Mutex handle here
	if (DuplicateHandle(service->serverProcess,
		service->hServiceExclusion, GetCurrentProcess(), &hMutexDup, 0, FALSE, DUPLICATE_SAME_ACCESS) == NULL){
			printf("\nERROR: Could not duplicate the server process' mutex handle! (%d)", GetLastError());
			return FALSE;
		}
	
	WaitForSingleObject(hMutexDup, INFINITE);//WaitForSingleObject(service->hServiceExclusion, INFINITE);
	nReq = service->nRequests;
	if (nReq >= MAX_REQUESTS_NR){ // se o nº máximo de pedidos foi alcançado, sair
		ReleaseMutex(hMutexDup);	//ReleaseMutex(service->hServiceExclusion);
		return FALSE;
	}
	service->requests[nReq].clientProcessId = clientProcId;
	service->requests[nReq].operation = operation;
	wcsncpy_s(service->requests[nReq].file, file, wcslen(file) + 1);
	service->nRequests++; 
	ReleaseMutex(hMutexDup);		//ReleaseMutex(service->hServiceExclusion);
	
	switch (operation)
	{
	case backup_operation:
		ProcessNextEntry(service, BackupFileFunction);
		break;
	case restore_operation:
		ProcessNextEntry(service, RestoreFileFunction);
		break;
	case exit_operation:
		CloseBackupService(service);
		break;
	default:
		break;
	}
	CloseHandle(hMutexDup);
	return TRUE;
}

BOOL CloseBackupService(HBACKUPSERVICE service){

	if (!UnmapViewOfFile((LPVOID)service)){
		printf("\nUnmapViewOfFile failed = ERROR %d", GetLastError());
		return FALSE;
	}
	CloseHandle(service);
	return TRUE;
}

//Cliente

// Obter referência para o serviço (instância de BackupServer)
HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){
	HBACKUPSERVICE pService;
	HANDLE hMapFile;

	hMapFile = (HBACKUPSERVICE)OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, serviceName);
	if (hMapFile == NULL)
	{
		printf("Could not open file mapping object (%d).\n",GetLastError());
		return NULL;
	}
	
	pService = (HBACKUPSERVICE) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BACKUPSERVICE));
	if (pService == NULL)
	{
		printf("Could not map view of file (%d).\n", GetLastError());
		return NULL;
	}

	return pService;
}

// Enviar um pedido de Backup de um ficheiro.
BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file){
	return SendNewRequest(service, GetCurrentProcessId(), backup_operation, file);
}

// Enviar um pedido de reposição de um ficheiro.
BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file){
	return SendNewRequest(service, GetCurrentProcessId(), restore_operation, file);
}

// Enviar pedido de terminação do serviço.
BOOL StopBackupService(TCHAR * serviceName){
	HBACKUPSERVICE service = (HBACKUPSERVICE)OpenFileMapping(PAGE_READWRITE, FALSE, serviceName);
	return SendNewRequest(service, GetCurrentProcessId(), exit_operation, NULL);
}
