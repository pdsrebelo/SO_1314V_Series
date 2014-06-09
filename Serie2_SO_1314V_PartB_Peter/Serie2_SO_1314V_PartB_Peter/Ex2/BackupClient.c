#include "BackupClient.h"

HBACKUPSERVICE OpenBackupService(TCHAR * serviceName){

}

BOOL BackupFile(HBACKUPSERVICE service, TCHAR * file){
	return FALSE;
}

BOOL RestoreFile(HBACKUPSERVICE service, TCHAR * file){
	return FALSE;
}

BOOL StopBackupService(TCHAR * serviceName){
	return FALSE;
}

BOOL CloseBackupService(HBACKUPSERVICE service){
	return FALSE;
}