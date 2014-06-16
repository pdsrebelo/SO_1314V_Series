#include "stdafx.h"
#include "..\BackupServerDll\BackupServer.h"

// Executado assim:
// BackupClient exit

int _tmain(int argc, _TCHAR* argv[])
{
	return StopBackupService(argv[1]);
}

