all: BackupServer.exe BackupClient.exe

BackupServer.exe: BackupServer.c BackupService.dll
    cl /W3 /Zi BackupServer.c BackupService.lib

BackupClient.exe: BackupClient.c BackupService.dll
    cl /W3 /Zi BackupClient.c BackupService.lib

BackupService.dll: BackupService.c
    cl /W3 /Zi /LD /D BACKUPSERVICE_DLL BackupService.c

clean:
    del BackupServer.exe BackupClient.exe BackupService.dll *.ilk *.obj *.pdb *.lib *.exp