#include "BackupServer.h"

HBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath){
	return;
}

BOOL ProcessNextEntry(HBACKUPSERVICE service, ProcessorFunc processor){
	return FALSE;
}

BOOL CloseBackupService(HBACKUPSERVICE service){
	return FALSE;
}