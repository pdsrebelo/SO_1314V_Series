#include "stdafx.h"
#include "..\BackupServerDll\BackupServerDll.h"

// Executado assim:
// TerminateService service_name

int _tmain(int argc, _TCHAR* argv[])
{
	return StopBackupService(argv[1]);
}

