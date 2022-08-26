#ifdef _WIN32
#include "WinServer.h"
#elif defined (__linux__)
#include "LinuxServer.h"
#endif


int main()
{
#ifdef _WIN32
	IServer* server = new WinServer("127.0.0.1", 54000);
#elif defined (__linux__)

	IServer* server = new LinuxServer("127.0.0.1", 54000);
#endif

	try
	{
		server->Init();
		server->Run();
	}
	catch (std::runtime_error& err)
	{
			std::cout << err.what() << std::endl;
	}



	return 0;
	
}

