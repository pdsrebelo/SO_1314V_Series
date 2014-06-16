#include "BackupServer.h"

HBACKUPSERVICE OpenBackupService(TCHAR *serviceName);

BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file);

BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file);

BOOL StopBackupService(TCHAR * serviceName);
