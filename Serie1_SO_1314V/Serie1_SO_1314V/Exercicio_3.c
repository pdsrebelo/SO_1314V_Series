#include "windows.h"
#include "stdio.h"
#include "psapi.h"
#include "tchar.h"

class Exercicio_3{



	/*3.	Realize  um  programa  que  apresente  na  consola  a  seguinte  informação  global  do  sistema  e 
			informação do processo em execução cujo process id é passado como argumento.
			Informação global:
			● Total de memória física existente
			● Total de memória física disponível
			● Total de memória virtual existente (física + page file)
			● Total de memória virtual disponível


			Informação local do processo:
			● Total de espaço de endereçamento virtual existente
			● Total de espaço de endereçamento virtual disponível
			● Dimensão do Working set
			● Indentificação  das  regiões  (conjunto  de  páginas  com  as  mesmas  características)  que 
			formam o espaço de endereçamento de um processo caracterizando­as de acordo com os 
			seguintes itens:

			○ endereço início e fim da região;
			○ tipo de acesso (read, write, execute);
			○ protecção com que a região foi alocada inicialmente;
			○ estado das páginas dentro da região (commit, free, reserve);
			○ tipo das páginas dentro da região (image, mapped, private);
			○ no  caso do tipo de páginas  ser image,  indicar o ficheiro com o caminho absoluto 
			no sistema de ficheiros de onde teve origem.

			Notas: 
			1. Confirme  os  seus  resultados  com  os  resultados  apresentados  pelas  ferramentas 
			ResourceMonitor e VMMap.
			2. Funções  interessantes  da API Windows: GetPerformanceInfo, GlobalMemoryStatusEx, 
			GetProcessMemoryInfo, VirtualQueryEx, EnumProcessModulesEx
			3. Algumas  das  funções  anteriores  implicam  a  ligação  com  a  biblioteca  psapi.lib  e  da 
			utilização do ficheiro psapi.h.

			*/



	int _tmain(int argc, _TCHAR* argv[])
	{
		DWORD pId = GetCurrentProcessId();
		_tprintf(_T("ProcessId = %d\n"), pId);

		
		
		return 0;
	}

};