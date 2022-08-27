#ifdef _WIN32
#include "CWinClient.h"
#elif defined (__linux__)
#include "CLinuxClient.h"
#endif


int main()
{

	//start menu

	std::string ipAddress{};
	int port{};

	std::cout << "Enter IP Address of the Server that you want to connect: ";

	std::cin >> ipAddress;

	std::cout << "Enter Server's listening Port that you want to connect: ";

	std::cin >> port;


#ifdef _WIN32
	IClient* client = new CWinClient(ipAddress, port);
#elif defined (__linux__)

	IClient* client = new CLinuxClient(ipAddress, port);
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
	delete client;
	
	return 0;

}

