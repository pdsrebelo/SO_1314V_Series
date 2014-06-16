#ifndef REPOSITORY_CLIENT
#define REPOSITORY_CLIENT
#endif

#include "BackupServer.h"

HBACKUPSERVICE REPOSITORY_CLIENT OpenBackupService(TCHAR *serviceName);

BOOL REPOSITORY_CLIENT BackupFile(HBACKUPSERVICE service, TCHAR * file);

BOOL REPOSITORY_CLIENT RestoreFile(HBACKUPSERVICE service, TCHAR * file);

BOOL REPOSITORY_CLIENT StopBackupService(TCHAR * serviceName);

