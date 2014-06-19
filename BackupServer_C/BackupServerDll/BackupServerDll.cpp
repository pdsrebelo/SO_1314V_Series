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

	HANDLE hfMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, maxSize, serviceName);
	if (hfMap == NULL){
		printf("\nERRO: Nao foi possivel chamar CreateFileMapping! #%d", GetLastError());
		return NULL;
	}

	backupService = (HBACKUPSERVICE)MapViewOfFile(hfMap, FILE_MAP_WRITE|FILE_MAP_READ|FILE_ALL_ACCESS, 0, 0, 0);
	if (backupService == NULL){
		printf("\nERRO: Nao foi possivel mapear em memoria! #%d",GetLastError());
		return NULL;
	}
	
	wcsncpy_s(backupService->fileStoragePath, repoPath, wcslen(repoPath)+1);
	backupService->nRequests = 0;
	wcsncpy_s(backupService->serviceName, serviceName, wcslen(serviceName)+1);
	backupService->hServiceExclusion = CreateMutex(NULL, FALSE, (LPCWSTR)"serverMutex"); // null para usar os security attributes por omissão
	backupService->serverProcess = GetCurrentProcess();

	for (i = 0; i < MAX_REQUESTS_NR; i++){
		backupService->requests[i].file = (wchar_t*)malloc(128);
	}

	if (backupService->hServiceExclusion == NULL){
		printf("\nERRO: Nao foi possivel criar o mutex! #%d", GetLastError());
		return NULL;
	}
	return backupService;
}

// Manda realizar a operacao escolhida, sobre o ficheiro
BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor){
	HBACKUPENTRY pRequest;
	DWORD nRequests;
	BOOL requestSuccess = FALSE; 
	WaitForSingleObject(service->hServiceExclusion, INFINITE);
	{
		nRequests = service->nRequests;
		// Ir buscar um pedido, para mandar processar
		if (nRequests > 0){
			pRequest = &(service->requests[nRequests]);
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
	ReleaseMutex(service->hServiceExclusion);
	return requestSuccess;
}

// Faz a reposicao (Restore) ou guarda uma copia do ficheiro (Backup), conforme o bool recebido.
// backupNrestore = true -> Backup
// backupNrestore = false -> Restore
BOOL CopyFile(BOOL backupNrestore, HBACKUPENTRY copyRequest){
	FILE * origin; FILE * destiny;
	char auxBuffer[MAX_PATH];
	char * separator = "/";
	char * repositoryPath = (char*)backupService->fileStoragePath;

	SIZE_T filePathSize = strlen(repositoryPath) + strlen(separator) + strlen((char*)copyRequest->file) + 1;
	SIZE_T strSize = 0;

	char * repoFileName = (char*)malloc(filePathSize);

	strSize += strlen((char*)backupService->fileStoragePath) + 1;
	strcpy_s(repoFileName, strSize, (char*)backupService->fileStoragePath);

	strSize += strlen(separator);
	strcat_s(repoFileName, strSize, separator);

	strSize += strlen((char*)copyRequest->file);
	strcat_s(repoFileName, strSize, (char*)copyRequest->file);


	if (fopen_s(&origin, backupNrestore?(char*)copyRequest->file:repoFileName, "rb") != 0){// abrir ficheiro origem para ler
		printf("\nERRO... O ficheiro %s nao existe!", backupNrestore ? (char*)copyRequest->file : repoFileName);
		return FALSE;
	}

	if (fopen_s(&destiny, backupNrestore ? repoFileName : (char*)copyRequest->file, "wb+") != 0){
		printf("\nERRO... Nao foi possivel criar o ficheiro %s ! Verifique se o caminho existe!", backupNrestore ? repoFileName : (char*)copyRequest->file);
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
	wcsncpy_s(service->requests[nReq].file, wcslen(file) + 1,file, wcslen(file) + 1);
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
