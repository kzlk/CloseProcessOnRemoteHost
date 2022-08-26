#ifdef _WIN32
#include "CWinClient.h"
#elif defined (__linux__)
#include "CLinuxClient.h"
#endif


int main()
{
#ifdef _WIN32
	IClient* client = new CWinClient("127.0.0.1", 54000);
#elif defined (__linux__)

	IServer* client = new CLinuxClient("127.0.0.1", 54000);
#endif


	try
	{
		client->Init();
		client->Run();

	}
	catch (std::runtime_error& err)
	{
		std::cout << err.what() << std::endl;
	}

	
	return 0;

}

