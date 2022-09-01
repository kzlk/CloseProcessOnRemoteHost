#ifdef __linux__
#include "CLinuxClient.h"

CClient::CClient(const std::string& ipAddress, const int port) : IClient(ipAddress, port){}

CClient::~CClient()
{
	Cleanup();
}

bool CClient::Init()
{

	if (CreateSocket()) return true;
	else return false;
}

void CClient::Run()
{
	isRepeat = true;

	int choice{};

	do
	{
		//handle receiving messages
		auto response = handler.clientHandler(sock);

		const auto handle = CError::statusHandler(response);

		if (handle == NULL) std::cout << "\nDo you want to continue? " << '\n';
		else if (handle == 1) { isRepeat = false; return; }
		else std::cout << "Something went wrong! Do you want to try again? " << '\n';

		while (choice != 1 && choice != 2)
		{
			std::cout << "Enter -> 1 to continue\nEnter -> 2 to exit\n";
			std::cout << '>';
			std::cin >> choice;

			switch (choice)
			{
			case 1:

				//send to server header "continue" for work
				if (handler.iContinue(sock) == -1)
				{
					std::cerr << "Error! Can not to continue work with server" << '\n';
					isRepeat = false;
				}
				isRepeat = true;
				break;

			case 2:

				//disconnect from server
				isRepeat = false;
				if (handler.iDisconect(sock) == -1)
					std::cerr << "Error! Can not to disconnect from server correctly" << '\n';
				break;

			default: std::cout << "Incorrect input! Try again!"; break;
			}

		}
		choice = 0;
	} while (isRepeat);

}

void CClient::Cleanup()
{
	close(sock);
}

bool CClient::CreateSocket()
{
	sock = socket(AF_INET, SOCK_STREAM, NULL);
	if (sock != -1)
	{

		// Bind the ip address and port to a socket
		sockaddr_in hint{};
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		//connect to server 
		int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));

		if (connResult == -1)
		{
			std::cerr << "Can't connect to server" << '\n';
			return false;
		}

	}
	else
	{
		std::cerr << "Cannot create a socket!" << '\n';
		return false;
	}

	return true;
}

#endif