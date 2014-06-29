/* COMPILE THIS FILE FROM MAKEFILE */
#include "BackupService.h"

int main(){
	HBACKUPSERVICE service = OpenBackupService("Nome");
	
	if (!BackupFile(service, NULL)){
		printf("Error!\n");
	}

	return 0;
}