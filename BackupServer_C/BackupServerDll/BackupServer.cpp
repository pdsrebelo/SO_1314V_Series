#include "stdafx.h"
#include <iostream>
#include <tchar.h>
#include "BackupServer.h"
/*
void receiveNewRequest(HBACKUPSERVICE service, BACKUPENTRY request){	//é usado mutex pois pode ser partilhado entre processos, ao contrário da critical section	WaitForSingleObject(service->hServiceExclusion, INFINITE);		//quando adquire exclusão, adiciona o pedido à fila		service->requests[service->nRequests] = request;		service->nRequests += 1;	ReleaseMutex(service->hServiceExclusion);}
*/
//TODO Conforme a operação indicada no pedido (next entry), processá-lo
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
				service->nRequests -= 1;
			}
		}
	}
	ReleaseMutex(service->hServiceExclusion);
	return requestSuccess;
}

//TODO Instanciar o serviço
HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath){
	HBACKUPSERVICE backupService;
	HANDLE hfMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0, serviceName);
	backupService = (HBACKUPSERVICE)MapViewOfFile(hfMap, PAGE_READWRITE, 0, 0, sizeof(BACKUPSERVICE));
	
	_tcscpy_s(backupService->fileStoragePath,repoPath);
	backupService->isOpen = TRUE;
	backupService->nRequests = 0;
	_tcscpy_s(backupService->serviceName, serviceName);
	//backupService->serverRequestProcessor = (ProcessorFunc)ProcessNextEntry;
	backupService->hServiceExclusion = CreateMutex(NULL, FALSE, serviceName); // null para usar os security attributes por omissão
	
	return backupService;
}

BOOL CloseBackupService(HBACKUPSERVICE service){
	WaitForSingleObject(service->hServiceExclusion, INFINITE);

	if (!UnmapViewOfFile((LPVOID)service))
		printf("\nUnmapViewOfFile failed = ERROR %s", GetLastError());
	else
		service->isOpen = FALSE;

	ReleaseMutex(service->hServiceExclusion);

	return !service->isOpen;
}