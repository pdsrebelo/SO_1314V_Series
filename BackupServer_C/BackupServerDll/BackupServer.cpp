#include "BackupServer.h"//TODO Instanciar o servi�oHBACKUPSERVICE CreateBackupService(TCHAR * serviceName, TCHAR * repoPath){}BOOL CloseBackupService(HBACKUPSERVICE service){}//TODO Conforme a opera��o indicada no pedido (next entry), process�-loBOOL ProcessNextEntry(HBACKUPSERVICE service){	WaitForSingleObject(service.hServiceExclusion, INFINITE);	{		// Ir buscar o primeiro pedido, para mandar processar	}	ReleaseMutex(service.hServiceExclusion);}