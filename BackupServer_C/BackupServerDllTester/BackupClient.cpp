#include "stdafx.h"
#include "..\BackupServerDll\BackupServerDll.h"

// Executado assim:
// BackupServer service_name operation_type {filepath}*

int _tmain(int argc, _TCHAR* argv[])
{
	//supondo que ja foi criado um servidor com o nome indicado por argumento:
	_TCHAR* op = argv[2];
	HBACKUPSERVICE pService = CreateBackupService(argv[1], (TCHAR*)"repositorioDoServidor");

	pService = OpenBackupService(argv[1]);

	if (wcscmp(op, L"b") == 0 || wcscmp(op, L"backup") == 0 || wcscmp(op, L"B") == 0){
		printf("\nOpcao BACKUP seleccionada...");
		SendNewRequest(pService, GetCurrentProcessId(), backup_operation, argv[3]);
	}
	if (wcscmp(op, L"r") == 0 || wcscmp(op, L"restore") == 0 || wcscmp(op, L"R") == 0){
		printf("\nOpcao RESTORE seleccionada...");
		SendNewRequest(pService, GetCurrentProcessId(), restore_operation, argv[3]);
	}
	if (wcscmp(op, L"e") == 0 || wcscmp(op, L"exit") == 0 || wcscmp(op, L"E") == 0){
		printf("\nOpcao EXIT seleccionada...");
		SendNewRequest(pService, GetCurrentProcessId(), exit_operation, argv[3]);
	}

	printf("\nPrima qualquer tecla para terminar...");
	getchar();
	return 0;
}