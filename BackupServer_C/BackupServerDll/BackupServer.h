#include <windows.h>
#include <process.h>
#include <stdio.h>

#define MAX_REQUESTS_NR 128		//número máximo de pedidos

typedef BOOL(*ProcessorFunc)(PBACKUPENTRY pentry);

typedef enum BACKUP_OPERATION{	//tipos de operações que se podem realizar sobre os ficheiros
	backup,
	restore,
	exit
};

typedef struct BACKUP_ENTRY{
	TCHAR * filePath;			//identificação do ficheiro
	DWORD clientProcessId;		//identificador do processo que originou o pedido
	HANDLE success;				//handle para indicar o resultado da operação
	HANDLE unsuccess;			//handle para indicar o resultado da operação
	BACKUP_OPERATION operation;	//tipo de operação a realizar
}BACKUPENTRY, *PBACKUPENTRY;

typedef struct H_BACKUP_SERVICE{
	TCHAR fileStoragePath[128];				//caminho para a pasta onde irão ser guardadas as cópias dos ficheiros
	BACKUPENTRY requests[MAX_REQUESTS_NR];
	HANDLE hServiceExclusion;				//handle usado para garantir exclusão ao acesso aos dados do serviço
	DWORD nRequests;						//número de pedidos existentes até ao momento (começa a 0. é usado como idx para os novos pedidos)
	ProcessorFunc serverRequestProcessor;	//função do servidor, a chamar cada vez que se for processar um pedido
}HBACKUPSERVICE, *PBACKUPSERVICE;


HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath);

BOOL CloseBackupService(HBACKUPSERVICE service);

BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor);