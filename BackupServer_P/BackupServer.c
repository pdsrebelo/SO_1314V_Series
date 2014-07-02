/* COMPILE THIS FILE FROM MAKEFILE */
#include "BackupService.h"

int main(int argc, char* argv[]){
	if (argc < 3){
		printf("\n++++ Please specify server name and a valid repository path ++++\n");
		return 0;
	}

	//HBACKUPSERVICE backupService = CreateBackupService("Server", "C:\\Users\\prebelo\\Desktop\\ISEL\\SO_1314V_Series\\BackupServer_P\\Repository");

	HBACKUPSERVICE backupService = CreateBackupService(argv[1], argv[2]);

	while (ProcessNextEntry(backupService, NULL) == TRUE);

	printf("\n++++ Server is Offline ++++\n");

	return 0;
}