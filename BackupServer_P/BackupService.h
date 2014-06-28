#include <Windows.h>
#include <stdio.h>

#ifdef BACKUPSERVICE_DLL
#define BACKUPSERVICE_API __declspec(dllexport)
#else
#define BACKUPSERVICE_API __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CLIENTS 16
#define MAX_REQUESTS 100

	enum OPERATION
	{
		Backup,
		Restore,
		Exit
	} operation;

	/*
	Cada entrada de pedido (do tipo BACKUPENTRY, a definir) deve conter os seguintes
	dados:
	- Identificação do ficheiro.
	- Identificador do processo que originou o pedido.
	- Dois HANDLEs para indicar o resultado da operação (sucesso ou insucesso).
	- O tipo de operação a realizar (backup, restore e exit).
	*/
	typedef struct{
		DWORD fileID;
		DWORD clientProcID;
		HANDLE sucess;
		HANDLE failure;
		enum OPERATION operation;
	}BACKUPENTRY, *PBACKUPENTRY;

	typedef struct{
		HANDLE hMapFile;
		BACKUPENTRY requests[MAX_REQUESTS];
		//SLOT slots[MAX_CLIENTS];
	}HBACKUPSERVICE, *PHBACKUPSERVICE;

	typedef BOOL(*ProcessorFunc) (PBACKUPENTRY pentry);
	BACKUPSERVICE_API BOOL CloseBackupService(HBACKUPSERVICE service);

	/*** Server ****/
	BACKUPSERVICE_API BOOL StopBackupService(TCHAR * serviceName);
	BACKUPSERVICE_API HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath);
	BACKUPSERVICE_API BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor);

	/*** Client ****/
	BACKUPSERVICE_API HBACKUPSERVICE OpenBackupService(TCHAR * serviceName);
	BACKUPSERVICE_API BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file);
	BACKUPSERVICE_API BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file);

#ifdef __cplusplus
}
#endif