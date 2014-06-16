#include "stdafx.h"
#include "BackupClient.h"

// TODO Obter referência para o serviço (instância de BackupServer)
HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){

	return NULL;
}

//TODO Adicionar um pedido de Backup de um ficheiro.
BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file){
	return FALSE;
}


//TODO Adicionar um pedido de reposição de um ficheiro.
BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file){
	return FALSE;
}


//TODO Pedido de terminação do serviço.
BOOL StopBackupService(TCHAR * serviceName){
	return FALSE;
}
