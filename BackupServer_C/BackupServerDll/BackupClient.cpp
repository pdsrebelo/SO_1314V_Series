#include "stdafx.h"
#include "BackupServer.h"

// Obter referência para o serviço (instância de BackupServer)
HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){
	HBACKUPSERVICE pService;
	// Como a instância já está mapeada, basta usar OpenFileMapping
	pService = (HBACKUPSERVICE) OpenFileMapping(PAGE_READWRITE, TRUE, serviceName);
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
