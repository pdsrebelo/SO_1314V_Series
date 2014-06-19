#include "stdafx.h"
#include "..\BackupServerDll\BackupServerDll.h"

// Executado assim:
// BackupServer service_name operation_type {filepath}*

/*
int _tmain(int argc, _TCHAR* argv[])
{
	//TODO
	HBACKUPSERVICE pService = OpenBackupService(argv[1]);

	if (strcmp((char*)argv[2], "backup") == 0)
		SendNewRequest(pService, GetCurrentProcessId(), backup_operation, argv[3]);
	if (strcmp((char*)argv[2], "restore") == 0)
		SendNewRequest(pService, GetCurrentProcessId(), restore_operation, argv[3]);
	if (strcmp((char*)argv[2], "exit") == 0)
		SendNewRequest(pService, GetCurrentProcessId(), exit_operation, argv[3]);

	
	return 0;
}

*/