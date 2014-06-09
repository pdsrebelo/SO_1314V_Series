#include <Windows.h>
#include "BackupServer.h"

/*
Outros processos (clientes) obtêm referências para o serviço (OpenBackupService), podendo
adicionar pedidos de backup (BackupFile), de reposição (RestoreFile) e de terminação (StopBackupService).
*/

HBACKUPSERVICE OpenBackupService(TCHAR * serviceName);

BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file);

BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file);

BOOL StopBackupService(TCHAR * serviceName);

BOOL CloseBackupService(HBACKUPSERVICE service);