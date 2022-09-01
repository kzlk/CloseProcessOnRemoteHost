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


	IClient* client = new CClient(ipAddress, port);

	if (client->Init()) client->Run();

	delete client;

#ifdef __linux___
	(void)getchar();
#elif defined(_WIN32)
	system("pause");
#else
#endif

	return 0;

}

