#include "stdafx.h"
#include "..\BackupServerDll\BackupServerDll.h"

// Executado assim:
// BackupServer service_name operation_type {filepath}*



int _tmain(int argc, _TCHAR* argv[])
{
	//TODO
	//supondo que ja foi criado um servidor com o nome indicado por argumento:
	HBACKUPSERVICE pService = CreateBackupService(argv[1], (TCHAR*)"/repositorioDoServidor");

	pService = OpenBackupService(argv[1]);

	if (wcscmp(argv[2], (wchar_t*)L"backup") == 0)
		SendNewRequest(pService, GetCurrentProcessId(), backup_operation, argv[3]);
	if (wcscmp(argv[2], (wchar_t*)L"restore") == 0)
		SendNewRequest(pService, GetCurrentProcessId(), restore_operation, argv[3]);
	if (wcscmp(argv[2], (wchar_t*)L"exit") == 0)
		SendNewRequest(pService, GetCurrentProcessId(), exit_operation, argv[3]);

	
	return 0;
}