// BackupServerDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BackupServer.h"

// SERVICE -> has a server

void receiveNewRequest(PBACKUPSERVICE service, BACKUPENTRY request){		//é usado mutex pois pode ser partilhado entre processos, ao contrário da critical section	WaitForSingleObject(service->hServiceExclusion, INFINITE);	{		//quando adquire exclusão, adiciona o pedido à fila		service->requests[service->nRequests] = request;		service->nRequests += 1;	}	ReleaseMutex(service->hServiceExclusion);}

void processRequest(PBACKUPSERVICE service, BACKUPENTRY request){
	//verificar qual o tipo de operação requerido
	switch (request.operation){
	case backup:
		break;
	case restore:
		break;
	case exit:
		service->
		break;
	default:
		throw ("Received invalid operation!");
	}
}