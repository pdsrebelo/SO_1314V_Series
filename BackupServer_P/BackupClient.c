/* COMPILE THIS FILE FROM MAKEFILE */
#include "BackupService.h"

int main(int argc, char* argv[]){
	if (argc < 3){
		printf("\n++++ Please specify server name and an operation ++++\n");
		return 0;
	}

	if (strncmp(argv[2], "backup", 6) != 0 && strncmp(argv[2], "restore", 7) != 0 && strncmp(argv[2], "exit", 5) != 0){
		printf("\n++++ Please select a valid operation ++++\n");
		return 0;
	}

	if ((strncmp(argv[2], "backup", 6) != 0 && strncmp(argv[2], "restore", 7) != 0) && argc < 4){
		printf("\n++++ Please specify a valid path to file ++++\n");
		return 0;
	}

	HBACKUPSERVICE service = OpenBackupService(argv[1]);
	
	if (strncmp(argv[2], "backup", 6) == 0){
		if (!BackupFile(service, argv[3])){
			printf("\nError doing the backup file!\n");
		}
	}
	else if (strncmp(argv[2], "restore", 7) == 0){
		if (!RestoreFile(service, argv[3])){
			printf("\nError doing the restore file!\n");
		}
	}
	else{ // Exit
		if (!CloseBackupService(service)){
			printf("\nError closing the connection!\n");
		}
		else{
			printf("\n++++ Server is Offline ++++\n");
		}
	}

	return 0;
}