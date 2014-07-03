#include "BackupService.h"

int main(int argc, char* argv[]){
	if (argc < 3){
		printf("\n++++ Please specify server name and an operation ++++\n");
		return 0;
	}

	HBACKUPSERVICE service = OpenBackupService(argv[1]);

	if (service != NULL){
		if (strncmp(argv[2], "backup", 6) == 0){
			if (argc > 3){
				if (!BackupFile(service, argv[3])){
					printf("\nError doing the backup file!\n");
				}
			}
			else{
				printf("\n++++ Please specify a valid path to file ++++\n");
			}
		}
		else if (strncmp(argv[2], "restore", 7) == 0){
			if (argc > 3){
				if (!RestoreFile(service, argv[3])){
					printf("\nError doing the backup file!\n");
				}
			}
			else{
				printf("\n++++ Please specify a valid path to file ++++\n");
			}
		}
		else if (strncmp(argv[2], "exit", 5) == 0){
			if (!CloseBackupService(service)){
				printf("\nError closing the connection!\n");
			}
		}
		else{
			printf("\n++++ Please specify a valid operation ++++\n");
		}
	}

	printf("\n++++ Client is Offline ++++\n");

	return 0;
}