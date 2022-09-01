#include <cstring>
#include "LinuxServer.h"

#ifdef __linux__
CServer::CServer(const std::string& ipAddress, const int port) : IServer(ipAddress, port) {}

CServer::~CServer()
{
	CServer::Cleanup();
}

bool CServer::Init()
{
	if (CreateSocket()) return true;
	return false;
}

void CServer::Run()
{

	clientSize = sizeof(client_addr);

	// Create the master file descriptor set and zero it
	FD_ZERO(&master);
	FD_SET(listening, &master);

	char host[NI_MAXHOST];	//client's remote name
	char service[NI_MAXSERV];	//service (i.e. port) the client is connected on

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);
	int max_sd, sd;
	running = true;
	int new_socket{};
	while (running)
	{
		//clear the socket set
		FD_ZERO(&master);

		//add master socket to set
		FD_SET(listening, &master);
		max_sd = listening;

		//add child sockets to set
		for (int i : client_socket)
		{
			//socket descriptor
			sd = i;

			//if valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &master);

			//highest file descriptor number, need it for the select function
			if (sd > max_sd)
				max_sd = sd;
		}

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		int socketCount = select(max_sd + 1, &master, NULL, NULL, NULL);

		if ((socketCount < 0) && (errno != EINTR))
		{
			printf("select error");
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(listening, &master))
		{
			new_socket = WaitForAConnection(listening);
			if (new_socket < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

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
			if (send(new_socket, header.getListProc, MESSAGE_HEADER_SIZE, 0) == E_RECV_SEND)
			{
				std::cerr << "Error in sending request to get list of processes from client" << '\n';
				return;
			}


			//add new socket to array of sockets
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				//if position is empty
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n", i);

					break;
				}
			}
		}
		else
		{

			//else its some IO operation on some other socket
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				sd = client_socket[i];

				if (FD_ISSET(sd, &master))
				{

					bool isContinue = true;

					getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0);

					while (isContinue)
					{

						E_CODE_MESSAGE stat = handler.clientHandler(new_socket, host, master);
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
							isContinue = false;
						}
						std::string shutdown{};
						std::cout << "To shutdown server : enter --> \\shutdown"
							"\nTo disconnect current client : enter --> \\disconnect"
							"\nTo continue --> press any key\n>";
						std::cin >> shutdown;
						if (shutdown == "\\shutdown")
						{
							running = false;
							isContinue = false;
							return;
						}
						if (shutdown == "\\disconnect")
						{
							running = true;
							isContinue = false;
							std::cout << "> Client " << host << (" with SOCKET # ") << new_socket << " disconnect because of critical error" << '\n';
							FD_CLR(new_socket, &master);
							close(new_socket);
							std::cout << "Waiting other client for a connection" << '\n';
							std::cout << "----------------------------------" << '\n';
						}
						else {
							//continue
							if (errGet == 0)
							{
								const auto isWorkContinue = handler.continueWork(new_socket);
								//the client disconnected of his own free will
								if (isWorkContinue == 1)
								{
									isContinue = false;
									//closesocket(sock);
									FD_CLR(new_socket, &master);
									std::cout << "> Client " << host << (" with SOCKET # ") << new_socket << " disconnected" << '\n';
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
									if (!handler.isGetListProc(new_socket))
										std::cerr << "Error in sending request to get list of processes from client" << '\n';
								}

								running = true;
							}
						}

					}
				}
			}
		}
	}

	FD_CLR(listening, &master);
	close(listening);
}

bool CServer::CreateSocket()
{

	// Create a socket
	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != -1)
	{
		// Bind the ip address and port to a socket
		sockaddr_in hint{};
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOK != -1)
		{
			int listenOK = listen(listening, SOMAXCONN);
			if (listenOK == -1)
			{
				std::cerr << "Error! Can not listen for incoming connections " << '\n';
				return false;
			}
		}
		else
		{
			std::cerr << "Error! Can not bind ip and port to socket" << '\n';
			return false;
		}
	}
	else
	{
		std::cerr << "Can not create listening socket" << '\n';
		return false;
	}

	return true;
}

int CServer::WaitForAConnection(int listening)
{
	int client = accept((listening), (sockaddr*)&client_addr, reinterpret_cast<socklen_t*>(&clientSize));
	return client;
}

void CServer::Cleanup()
{
	const std::string msg = "Server is shutting down. Goodbye\r\n";

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		int sock = client_socket[i];

		if (FD_ISSET(sock, &master))
		{
			send(sock, header.closeServer, MESSAGE_HEADER_SIZE, 0);
			send(sock, msg.c_str(), msg.size() + 1, 0);
			close(sock);
			client_socket[i] = 0;
		}

	}
}

#endif



