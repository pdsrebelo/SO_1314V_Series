#ifndef REPOSITORY_SERVER
	#define REPOSITORY_SERVER __declspec(dllimport)
#else
	#define REPOSITORY_SERVER __declspec(dllexport)
#endif

#include <windows.h>

#define STR_MAX_SIZE 128
#define MAX_REQUESTS_NR 128		//número máximo de pedidos

typedef enum BACKUP_OPERATION{	//tipos de operações que se podem realizar sobre os ficheiros
	backup_operation,
	restore_operation,
	exit_operation
};

typedef struct BACKUP_ENTRY{
	DWORD clientProcessId;		//identificador do processo que originou o pedido
	TCHAR file[STR_MAX_SIZE];	//identificação do ficheiro
	HANDLE success;				//handle para indicar o resultado da operação
	HANDLE unsuccess;			//handle para indicar o resultado da operação
	BACKUP_OPERATION operation;	//tipo de operação a realizar
}BACKUPENTRY, *HBACKUPENTRY;

typedef BOOL(*ProcessorFunc)(HBACKUPENTRY pentry);

typedef struct BACKUP_SERVICE{
	TCHAR serviceName[STR_MAX_SIZE];
	TCHAR fileStoragePath[STR_MAX_SIZE];				//caminho para a pasta onde irão ser guardadas as cópias dos ficheiros
	BACKUPENTRY requests[MAX_REQUESTS_NR];
	HANDLE hServiceExclusion;				//handle usado para garantir exclusão ao acesso aos dados do serviço
	DWORD nRequests;						//número de pedidos existentes até ao momento (começa a 0. é usado como idx para os novos pedidos)	
	HANDLE serverProcess;					//handle para o processo que criou o servidor
} BACKUPSERVICE, *HBACKUPSERVICE;

HBACKUPSERVICE REPOSITORY_SERVER CreateBackupService(TCHAR * serviceName, TCHAR * repoPath);

BOOL REPOSITORY_SERVER CloseBackupService(HBACKUPSERVICE service);

BOOL REPOSITORY_SERVER ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor);

BOOL REPOSITORY_SERVER RestoreFileFunction(HBACKUPENTRY pentry);

BOOL REPOSITORY_SERVER BackupFileFunction(HBACKUPENTRY pentry);

BOOL REPOSITORY_SERVER SendNewRequest(HBACKUPSERVICE service, DWORD clientProcId, BACKUP_OPERATION operation, TCHAR * file);

HBACKUPSERVICE REPOSITORY_SERVER OpenBackupService(TCHAR *serviceName);

BOOL REPOSITORY_SERVER BackupFile(HBACKUPSERVICE service, TCHAR * file);

BOOL REPOSITORY_SERVER RestoreFile(HBACKUPSERVICE service, TCHAR * file);

BOOL REPOSITORY_SERVER StopBackupService(TCHAR * serviceName);