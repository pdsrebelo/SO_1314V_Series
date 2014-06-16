#include <windows.h>
#include <process.h>
#include <stdio.h>
typedef struct H_BACKUP_SERVICE{}HBACKUPSERVICE, *PBACKUPSERVICE;

typedef enum BACKUP_OPERATION{
	backup,
	restore,
	exit
};

/*
Cada  entrada  de  pedido (do  tipo  BACKUPENTRY, a  definir)  deve  conter  os  seguintes 
dados : identificação  do  ficheiro;  Identificador  do  processo  que  originou  o  pedido;  dois  HANDLEs 
para  indicar  o  resultado  da  operação (sucesso  ou  insucesso);  o  tipo  de  operação  a  realizar 
(backup, restore e exit).*/
typedef struct BACKUP_ENTRY{
	TCHAR * fileName;			//identificação do ficheiro
	DWORD clientProcessId;		//identificador do processo que originou o pedido
	HANDLE succes;				//handle para indicar o resultado da operação
	HANDLE unsuccess;			//handle para indicar o resultado da operação
	BACKUP_OPERATION operation;//tipo de operação a realizar
}BACKUPENTRY, *PBACKUPENTRY;

typedef BOOL(*ProcessorFunc)(PBACKUPENTRY pentry);