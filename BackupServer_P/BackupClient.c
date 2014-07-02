/* COMPILE THIS FILE FROM MAKEFILE */
#include "BackupService.h"

int main(){
	HBACKUPSERVICE service = OpenBackupService("BackupServer");
	
	if (!BackupFile(service, NULL)){
		printf("Error!\n");
	}

	return 0;
}