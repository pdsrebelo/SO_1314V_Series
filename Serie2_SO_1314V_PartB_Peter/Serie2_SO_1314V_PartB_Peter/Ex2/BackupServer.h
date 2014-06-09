#include <Windows.h>

typedef struct{

}HBACKUPSERVICE, *PHBACKUPSERVICE;

typedef struct{

}BACKUPENTRY, *PBACKUPENTRY;

typedef BOOL(*ProcessorFunc) (PBACKUPENTRY pentry);

HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath);

BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor);

BOOL CloseBackupService(HBACKUPSERVICE service);