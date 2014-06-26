#include "stdafx.h"
#include "..\BackupServerDll\BackupServerDll.h"

// Executado assim:
// TerminateService service_name


/*
int _tmain(int argc, _TCHAR* argv[])
{
	BOOL success = FALSE;
	
	// ... Tirar esta parte (só para debug)
	//supondo que outro processo ja criou o serviço
	HBACKUPSERVICE pService = CreateBackupService(argv[1], (TCHAR*)"repositorioDoServidor");
	// ... 

	if (success = StopBackupService(argv[1]))
		printf("\nSucesso: O servico de Backup esta PARADO!");
	else 
		printf("\nERRO: O servico de Backup ainda esta ACTIVO!");

	printf("\n\nPrima qualquer tecla para terminar...");
	getchar();
	return success;
}*/
