#include <Windows.h>

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
	DWORD procID;
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

HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath);

BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor);

BOOL CloseBackupService(HBACKUPSERVICE service);