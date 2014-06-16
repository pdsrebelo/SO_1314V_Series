#include <windows.h>
#include <process.h>
#include <stdio.h>

#define MAX_REQUESTS_NR 128		//número máximo de pedidos

typedef enum BACKUP_OPERATION{	//tipos de operações que se podem realizar sobre os ficheiros
	backup_operation,
	restore_operation,
	exit_operation
};

typedef struct BACKUP_ENTRY{
	TCHAR * file;				//identificação do ficheiro
	DWORD clientProcessId;		//identificador do processo que originou o pedido
	HANDLE success;				//handle para indicar o resultado da operação
	HANDLE unsuccess;			//handle para indicar o resultado da operação
	BACKUP_OPERATION operation;	//tipo de operação a realizar
}BACKUPENTRY, *HBACKUPENTRY;

typedef BOOL(*ProcessorFunc)(HBACKUPENTRY pentry);

typedef struct BACKUP_SERVICE{
	TCHAR serviceName[128];
	TCHAR fileStoragePath[128];				//caminho para a pasta onde irão ser guardadas as cópias dos ficheiros
	BACKUPENTRY requests[MAX_REQUESTS_NR];
	HANDLE hServiceExclusion;				//handle usado para garantir exclusão ao acesso aos dados do serviço
	DWORD nRequests;						//número de pedidos existentes até ao momento (começa a 0. é usado como idx para os novos pedidos)	
} BACKUPSERVICE, *HBACKUPSERVICE;

HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath);

BOOL CloseBackupService(HBACKUPSERVICE service);

BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor);

BOOL RestoreFileFunction(HBACKUPENTRY pentry);

BOOL BackupFileFunction(HBACKUPENTRY pentry);

BOOL SendNewRequest(HBACKUPSERVICE service, HBACKUPENTRY request);