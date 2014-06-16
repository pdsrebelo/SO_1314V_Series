#include "stdafx.h"
#include <iostream>
#include <tchar.h>
#include "BackupServer.h"
#include <cstdio>

HBACKUPSERVICE backupService;

//TODO Instanciar o serviço
HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath){
	
	HANDLE hfMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0, serviceName);
	backupService = (HBACKUPSERVICE)MapViewOfFile(hfMap, PAGE_READWRITE, 0, 0, sizeof(BACKUPSERVICE));

	_tcscpy_s(backupService->fileStoragePath, repoPath);
	backupService->nRequests = 0;
	_tcscpy_s(backupService->serviceName, serviceName);
	backupService->hServiceExclusion = CreateMutex(NULL, FALSE, serviceName); // null para usar os security attributes por omissão

	return backupService;
}

//TODO Copia ficheiro
BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor){
	HBACKUPENTRY pRequest;
	DWORD nRequests;
	BOOL requestSuccess = FALSE; 
	WaitForSingleObject(service->hServiceExclusion, INFINITE);
	{
		nRequests = service->nRequests;
		// Ir buscar o primeiro pedido, para mandar processar
		if (nRequests > 0){
			pRequest = &(service->requests[nRequests]);
			requestSuccess = processor(pRequest);
			// Se houve sucesso no processamento do pedido, decrementar nº de pedidos
			if (requestSuccess){
				service->nRequests --;
			}
		}
	}
	ReleaseMutex(service->hServiceExclusion);
	return requestSuccess;
}

BOOL copyFileToRepository(HBACKUPENTRY pentry){
	FILE * origin; FILE * destiny;
	char auxBuffer[MAX_PATH];
	char * separator = "/";
	char * repositoryPath = (char*)backupService->fileStoragePath;

	SIZE_T filePathSize = strlen(repositoryPath) + strlen(separator) + strlen((char*)pentry->file) + 1; 
	SIZE_T strSize = 0;

	char * newFileName = (char*)malloc(filePathSize);

	strSize += strlen((char*)backupService->fileStoragePath) + 1;
	strcpy_s(newFileName, strSize, (char*)backupService->fileStoragePath);

	strSize += strlen(separator);
	strcat_s(newFileName, strSize, separator);

	strSize += strlen((char*)backupService->fileStoragePath);
	strcat_s(newFileName, strSize, (char*)pentry->file);

	if (fopen_s(&origin, (char*)backupService->fileStoragePath, "rb") != 0){// abrir ficheiro origem para ler
		printf("\nERRO... O ficheiro %s nao existe!", backupService->fileStoragePath);
		return FALSE;
	}

	if (fopen_s(&destiny, (char*)pentry->file, "wb+") != 0){
		printf("\nERRO... Nao foi possivel criar o ficheiro %s ! Verifique se o caminho existe!", pentry->file);
		return FALSE;
	}
	
	while (fgets(auxBuffer, sizeof(auxBuffer), origin) != NULL){
		fprintf_s(destiny, auxBuffer);
	}

	fclose(origin);
	fclose(destiny);
	return TRUE;
}

BOOL NewRequest(HBACKUPSERVICE service, HBACKUPENTRY request){
	DWORD nReq;
	WaitForSingleObject(service->hServiceExclusion, INFINITE);
	nReq = service->nRequests;
	if (nReq >= MAX_REQUESTS_NR){ // se o nº máximo de pedidos foi alcançado, sair
		ReleaseMutex(service->hServiceExclusion);
		return FALSE;
	}
	service->requests[nReq] = *request;
	service->nRequests ++;
	ReleaseMutex(service->hServiceExclusion);
	return TRUE;
}

BOOL CloseBackupService(HBACKUPSERVICE service){

	if (!UnmapViewOfFile((LPVOID)service)){
		printf("\nUnmapViewOfFile failed = ERROR %s", GetLastError());
		return FALSE;
	}
	CloseHandle(service);
	return TRUE;
}