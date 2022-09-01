#ifdef _WIN32
#include "WinServer.h"
#elif defined (__linux__)
#include "LinuxServer.h"
#endif

int main()
{

	IServer* server = new CServer("127.0.0.1", 54000);

	if (server->Init()) server->Run();

	delete server;


#ifdef __linux___
	(void)getchar();
#elif defined(_WIN32)
	system("pause");
#else
#endif

	return 0;

	
}

