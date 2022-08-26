#include "LinuxServer.h"

#ifdef __linux__
LinuxServer::LinuxServer(const std::string ipAddress, const int port): IServer(ipAddress, port)  {}

LinuxServer::~LinuxServer()
{
	LinuxServer::Cleanup();
}

bool LinuxServer::Init()
{
	listening = (int)CreateSocket();

	if(listening == -1) throw std::runtime_error("Can't create a socket! Quitting");

	return listenig != -1
}

void LinuxServer::Run()
{
	//create listening socket
	//int listening = (int)CreateSocket();
	//if (listening == INVALID_SOCKET) { return; }
	//wait for a connection 
	//loop for connection (select)

	clientSize = sizeof(client_addr);

	// Create the master file descriptor set and zero it

	FD_ZERO(&master);

	FD_SET(listening, &master);
	char host[NI_MAXHOST];	//client's remote name
	char service[NI_MAXSERV];	//service (i.e. port) the client is connected on

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	bool running = true;

	while (running)
	{

		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			int sock = copy.fd_array[i];

			if (sock == listening)
			{

				// Accept a new connection
				
				int client = (int)WaitForAConnection((void*)listening, clientSize);

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
				if (send(client, header.getListProc, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
				{
					std::cerr << "Error in sending request to get list of processes from client" << '\n';
					return;
				}

			}
			else // an inbound message
			{
				//inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
				getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0);

				auto stat = handler.clientHandler(sock, host, master);

				//statusHandler(stat);
			}
		}
	}

	FD_CLR(listening, &master);
	close(listening);
}

void* LinuxServer::CreateSocket()
{

	// Create a socket
	int listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != -1)
	{
		// Bind the ip address and port to a socket
		sockaddr_in hint{};
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		//hint.sin_addr.S_addr = INADDR_ANY;
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOK != SOCKET_ERROR)
		{
			int listenOK = listen(listening, SOMAXCONN);
			if (listenOK == SOCKET_ERROR)
			{
				return (void*)-1;
			}
		}
		else
		{
			return (void*)-1;
		}
	}

	return (void*)listening;

}


void* LinuxServer::WaitForAConnection(void* listening, int& clientSiz)
{
	int client = accept((int)listening, (sockaddr*)&client_addr, &clientSize);
	return (void*)client;
}

void LinuxServer::Cleanup()
{
	const std::string msg = "Server is shutting down. Goodbye\r\n";
	while (master.fd_count > 0)
	{
		const int sock = master.fd_array[0];
		send(sock, msg.c_str(), msg.size() + 1, 0);
		FD_CLR(sock, &master);
		close(sock);

	}
}


#endif