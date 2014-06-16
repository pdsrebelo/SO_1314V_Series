#include "stdafx.h"
#include <iostream>
#include <tchar.h>
#include "BackupServerDll.h"
#include <cstdio>

HBACKUPSERVICE backupService;

// Instanciar o serviço
HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath){
	
	HANDLE hfMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0, serviceName);
	backupService = (HBACKUPSERVICE)MapViewOfFile(hfMap, PAGE_READWRITE, 0, 0, sizeof(BACKUPSERVICE));

	_tcscpy_s(backupService->fileStoragePath, repoPath);
	backupService->nRequests = 0;
	_tcscpy_s(backupService->serviceName, serviceName);
	backupService->hServiceExclusion = CreateMutex(NULL, FALSE, serviceName); // null para usar os security attributes por omissão

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
			}
		}
	}
	ReleaseMutex(service->hServiceExclusion);
	return requestSuccess;
}

// Repoe o ficheiro original (copia novamente)
BOOL RestoreFileFunction(HBACKUPENTRY pentry){
	FILE * origin; FILE * destiny;
	char auxBuffer[MAX_PATH];
	char * separator = "/";
	char * repositoryPath = (char*)backupService->fileStoragePath;

	SIZE_T filePathSize = strlen(repositoryPath) + strlen(separator) + strlen((char*)pentry->file) + 1;
	SIZE_T strSize = 0;

	char * backedUpFileName = (char*)malloc(filePathSize);

	strSize += strlen((char*)backupService->fileStoragePath) + 1;
	strcpy_s(backedUpFileName, strSize, (char*)backupService->fileStoragePath);

	strSize += strlen(separator);
	strcat_s(backedUpFileName, strSize, separator);

	strSize += strlen((char*)backupService->fileStoragePath);
	strcat_s(backedUpFileName, strSize, (char*)pentry->file);

	if (fopen_s(&origin, backedUpFileName, "rb") != 0){// abrir ficheiro origem para ler
		printf("\nERRO... O ficheiro %s nao existe!", backedUpFileName);
		return FALSE;
	}

	if (fopen_s(&destiny, (char*)pentry->file, "wb+") != 0){
		printf("\nERRO... Nao foi possivel repôr o ficheiro %s ! Verifique se o caminho ainda existe!", pentry->file);
		return FALSE;
	}

	while (fgets(auxBuffer, sizeof(auxBuffer), origin) != NULL){
		fprintf_s(destiny, auxBuffer);
	}

	fclose(origin);
	fclose(destiny);
	return TRUE;
}

//Copia ficheiro
BOOL BackupFileFunction(HBACKUPENTRY pentry){
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

	strSize += strlen((char*)pentry->file);
	strcat_s(newFileName, strSize, (char*)pentry->file);

	if (fopen_s(&origin, (char*)pentry->file, "rb") != 0){// abrir ficheiro origem para ler
		printf("\nERRO... O ficheiro %s nao existe!", pentry->file);
		return FALSE;
	}

	if (fopen_s(&destiny, newFileName, "wb+") != 0){
		printf("\nERRO... Nao foi possivel criar o ficheiro %s ! Verifique se o caminho existe!", newFileName);
		return FALSE;
	}
	
	while (fgets(auxBuffer, sizeof(auxBuffer), origin) != NULL){
		fprintf_s(destiny, auxBuffer);
	}

	fclose(origin);
	fclose(destiny);
	return TRUE;
}

BOOL SendNewRequest(HBACKUPSERVICE service, DWORD clientProcId, BACKUP_OPERATION operation, TCHAR * file){
	DWORD nReq;
	WaitForSingleObject(service->hServiceExclusion, INFINITE);
	nReq = service->nRequests;
	if (nReq >= MAX_REQUESTS_NR){ // se o nº máximo de pedidos foi alcançado, sair
		ReleaseMutex(service->hServiceExclusion);
		return FALSE;
	}
	service->requests[nReq].clientProcessId = clientProcId;
	service->requests[nReq].operation = operation;
	service->requests[nReq].file = file;
	service->nRequests ++;
	ReleaseMutex(service->hServiceExclusion);
	
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
		printf("\nUnmapViewOfFile failed = ERROR %s", GetLastError());
		return FALSE;
	}
	CloseHandle(service);
	return TRUE;
}

//Cliente

// Obter referência para o serviço (instância de BackupServer)
HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){
	HBACKUPSERVICE pService;
	// Como a instância já está mapeada, basta usar OpenFileMapping
	pService = (HBACKUPSERVICE)OpenFileMapping(PAGE_READWRITE, TRUE, serviceName);
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
