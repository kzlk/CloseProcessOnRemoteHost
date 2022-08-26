#include "CWinClient.h"

#include <utility>

CWinClient::CWinClient(std::string ipAddress, const int port): IClient(std::move(ipAddress), port) {}

CWinClient::~CWinClient()
{
	CWinClient::Cleanup();
}

bool CWinClient::Init()
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != NULL)
	{
		throw std::runtime_error("Can't Initialize WinSock! Quitting");
	}

	CreateSocket();

	return true ;
}

void CWinClient::Run()
{

	 isRepeat = true;

	  int choice{};
	//TODO: exit from cycle

	do
	{
		//handle receiving messages
		auto response = handler.clientHandler(sock);

		const auto handle = catchy.statusHandler(response);
		if (handle == NULL)
			std::cout << "\nDo you want to continue? " << '\n';
		else
			std::cout << "Something went wrong! Do you want to try again? " << '\n';
		std::cout << "Enter -> 1 to continue\nEnter -> 2 to exit\n";

		while (choice != 1 && choice != 2)
		{
			std::cout << '>';
			std::cin >> choice;

			switch (choice)
			{
			case 1:
				isRepeat = true;
				break;
			case 2:
				isRepeat = false;
				handler.iDisconect(sock);
				break;
			default: std::cout << "Incorrect input! Try again!"; break;
			}

		}

	} while (isRepeat);


}

void CWinClient::Cleanup()
{
	closesocket(sock);
	WSACleanup();
}

void CWinClient::CreateSocket()
{
	sock = socket(AF_INET, SOCK_STREAM, NULL);
	if (sock != INVALID_SOCKET)
	{

		// Bind the ip address and port to a socket
		sockaddr_in hint{};
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		//hint.sin_addr.S_un.S_addr = INADDR_ANY;
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		//connect to server 

		int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));

		if (connResult == SOCKET_ERROR) throw std::runtime_error("Can't connect to server");

	}
	else throw std::runtime_error("Cannot create a socket!");

}

