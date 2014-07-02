/* COMPILE THIS FILE FROM MAKEFILE */
#include "BackupService.h"

int main(){
	HBACKUPSERVICE backupService = CreateBackupService("BackupServer", "C:\\Users\\prebelo\\Desktop\\ISEL\\SO_1314V_Series\\BackupServer_P\\Repository");

	while (ProcessNextEntry(backupService, NULL) == TRUE);

	printf("++++ Server is Offline ++++\n");

	return 0;
}