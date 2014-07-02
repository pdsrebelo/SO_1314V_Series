/* COMPILE THIS FILE FROM MAKEFILE */
#include "BackupService.h"

int main(int argc, char **argv){
	if (argc < 3){
		printf("++++ Please specify server name and an operation ++++\n");
		return 0;
	}

	if (strncmp(argv[2], "backup", 10) != 0 || strncmp(argv[2], "backup", 10) != 0 || strncmp(argv[2], "exit", 10) != 0){
		printf("++++ Please select a valid operation ++++\n");
		return 0;
	}

	if ((strncmp(argv[2], "backup", 10) == 0 || strncmp(argv[2], "restore", 10) == 0) && argc < 4){
		printf("++++ Please specify a valid path to file ++++\n");
		return 0;
	}

	//HBACKUPSERVICE service = OpenBackupService("BackupServer");

	HBACKUPSERVICE service = OpenBackupService(argv[1]);
	
	if (strncmp(argv[2], "backup", 10) == 0){
		if (!BackupFile(service, argv[3])){
			printf("Error doing the backup file!\n");
		}
	}
	else if (strncmp(argv[2], "restore", 10) == 0){
		if (!RestoreFile(service, argv[3])){
			printf("Error doing the restore file!\n");
		}
	}
	else{ // Exit
		if (!CloseBackupService(service)){
			printf("Error closing the connection!\n");
		}
	}

	return 0;
}