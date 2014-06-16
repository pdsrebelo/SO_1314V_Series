#include "stdafx.h"
#include "BackupClient.h"


// Obter referência para o serviço (instância de BackupServer)
HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){
	HBACKUPSERVICE pService;
	// Como a instância já está mapeada, basta usar OpenFileMapping
	pService = (HBACKUPSERVICE) OpenFileMapping(PAGE_READWRITE, TRUE, serviceName);
	return pService;
}

// Enviar um pedido de Backup de um ficheiro.
BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file){
	HBACKUPENTRY pentry;
	pentry->file = file;
	pentry->clientProcessId = GetCurrentProcessId();
	pentry->operation = backup_operation;
	return SendNewRequest(service, pentry);
}


// Enviar um pedido de reposição de um ficheiro.
BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file){
	HBACKUPENTRY pentry;
	pentry->file = file;
	pentry->clientProcessId = GetCurrentProcessId();
	pentry->operation = restore_operation;
	return SendNewRequest(service, pentry);
}


// Enviar pedido de terminação do serviço.
BOOL StopBackupService(TCHAR * serviceName){
	HBACKUPENTRY pentry;
	HBACKUPSERVICE service;
	pentry->file = NULL;
	pentry->clientProcessId = GetCurrentProcessId();
	pentry->operation = exit_operation;
	service = (HBACKUPSERVICE)OpenFileMapping(PAGE_READWRITE, FALSE, serviceName);
	return SendNewRequest(service, pentry);
}
