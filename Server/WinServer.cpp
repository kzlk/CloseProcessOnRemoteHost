#ifdef  _WIN32
#include "WinServer.h"

#include <thread>
#include <utility>

CServer::CServer(std::string ipAddress, const int port): IServer(std::move(ipAddress), port)
{}

CServer::~CServer()
{
	CServer::Cleanup();
}

bool CServer::Init()
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk == NULL)
	{
		if (CreateSocket()) return true;
		return false;
		
	}
	std::cerr << "Can't Initialize WinSock! Quitting" << '\n';
	return false;
}

bool CServer::CreateSocket()
{
	// Create a socket
	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET)
	{
		// Bind the ip address and port to a socket
		sockaddr_in hint{};
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOK != SOCKET_ERROR)
		{
			// Tell Winsock the socket is for listeningSock
			int listenOK = listen(listening, SOMAXCONN);
			if (listenOK == SOCKET_ERROR)
			{
				std::cerr << "Error! Can not listen for incoming connections " << '\n';
				return false;
			}
		}
		else
		{
			std::cerr << "Error! Can not bind ip and port to socket" << '\n';
			return false ;
		}
	}
	else
	{
		std::cerr << "Can not create listening socket" << '\n';
		return false;
	}

	return true;
}

SOCKET CServer::WaitForAConnection(SOCKET listening)
{
	SOCKET client = accept((SOCKET)listening, (sockaddr*)&client_addr, &clientSize);
	return client;
}

//TODO: close server correctly
void CServer::Run()
{

	clientSize = sizeof(client_addr);

	// Create the master file descriptor set and zero it
	FD_ZERO(&master);

	FD_SET(listening, &master);

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	running = true;
	SOCKET client{};
	while (running)
	{

		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
		
		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{

               	// Accept a new connection
				//SOCKET client = accept(listening, (sockaddr*)&client_addr, &clientSize);
				client = WaitForAConnection(listening);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				if (getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
				{
					std::cout << host << " connected on port " << service << std::endl;
				}
				else
				{
					inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
					std::cout << host << " connected on port " << ntohs(client_addr.sin_port) << '\n';
				}

				//send request to get list of processes from client
				/*if (send(client, header.getListProc, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
				{
					std::cerr << "Error in sending request to get list of processes from client" << '\n';
				}*/

				//send request to get list of processes from client
				if(!handler.isGetListProc(client))
					std::cerr << "Error in sending request to get list of processes from client" << '\n';
			}
			else // an inbound message
			{
				bool isContinue = true;
				
				getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0);

				while (isContinue)
				{

					E_CODE_MESSAGE stat = handler.clientHandler(sock, host, master);
					const int errGet = CError::statusHandler(stat);

					//server stop
					if (errGet == 1)
					{
						running = false;
						isContinue = false;
						return;
					}

					if (errGet == -1)
					{
						std::cout << "Critical error occur" << '\n';
						running = true;
						isContinue = true;
					}
					std::string shutdown{};
					std::cout << "To shutdown server : enter --> \\shutdown"
					"\nTo disconnect current client : enter --> \\disconnect"
					"\nTo continue --> press any key\n>";
					std::cin >> shutdown;
					if(shutdown == "\\shutdown")
					{
						running = false;
						isContinue = false;
						return;
					}
					if(shutdown ==  "\\disconnect")
					{
						running = true;
						isContinue = false;
						std::cout << "> Client " << host << (" with SOCKET # ") << sock << " disconnect because of critical error" << '\n';
						FD_CLR(sock, &master);
						closesocket(sock);
						std::cout << "Waiting other client for a connection" << '\n';
						std::cout << "----------------------------------" << '\n';
					}
					else {
						//continue
						if (errGet == 0)
						{
							const auto isWorkContinue = handler.continueWork(sock);
							//disconnect client
							if (isWorkContinue == 1)
							{
								isContinue = false;
								//closesocket(sock);
								FD_CLR(sock, &master);
								std::cout << "> Client " << host << (" with SOCKET # ") << sock << " disconnected" << '\n';
								std::cout << "Waiting other client for a connection" << '\n';
								std::cout << "----------------------------------" << '\n';
							}
							//client still working
							else if (isWorkContinue == 0)
							{
								isContinue = true;
								std::cout << "Client " << host << " continue working in server" << '\n';
								std::cout << "--------------------------------------" << '\n';
								//send request to get list of processes from client
								if (!handler.isGetListProc(client))
									std::cerr << "Error in sending request to get list of processes from client" << '\n';
							}

							running = true;
						}
					}

				}
			}
		}
	}

	FD_CLR(listening, &master);
	closesocket(listening);

}

void CServer::Cleanup()
{
	const std::string msg = "Server is shutting down. Goodbye\r\n";
	while (master.fd_count > 0)
	{
		const SOCKET sock = master.fd_array[0];
		send(sock, header.closeServer, MESSAGE_HEADER_SIZE, 0);
		send(sock, msg.c_str(), msg.size() + 1, 0);
		FD_CLR(sock, &master);

		closesocket(sock);

	}
	WSACleanup();
}

#endif