#include "WinServer.h"

#include <thread>
#include <utility>

WinServer::WinServer(std::string ipAddress, const int port): IServer(std::move(ipAddress), port)
{}

WinServer::~WinServer()
{
	WinServer::Cleanup();
}

bool WinServer::Init()
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != NULL)
	{
		throw std::runtime_error("Can't Initialize WinSock! Quitting");
		return false;
	}

	 listening = (SOCKET)CreateSocket();
	if (listening == INVALID_SOCKET)
	{
		throw std::runtime_error("Can't create a socket! Quitting");
		//std::cerr << "Can't create a socket! Quitting" << '\n'; return;
	}

	return (wsOk == NULL) && (listening != (INVALID_SOCKET));
}

void* WinServer::CreateSocket()
{
	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET)
	{

		// Bind the ip address and port to a socket
		sockaddr_in hint{};
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		//hint.sin_addr.S_un.S_addr = INADDR_ANY;
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOK != SOCKET_ERROR)
		{
			// Tell Winsock the socket is for listening 
			int listenOK = listen(listening, SOMAXCONN);
			if (listenOK == SOCKET_ERROR)
			{
				return (void*) (-1);
			}
		}
		else
		{
			return (void*) (-1) ;
		}
	}

	return (void*)listening;
}

void* WinServer::WaitForAConnection(void* listening, int& clientSiz)
{
	SOCKET client = accept((SOCKET)listening, (sockaddr*)&client_addr, &clientSize);
	return (void*)client;
}

//TODO: close server correctly
void WinServer::Run()
{
	//create listening socket
	//SOCKET listening = (SOCKET)CreateSocket();
	//if (listening == INVALID_SOCKET) 
	//{ std::cerr << "Can't create a socket! Quitting" << '\n'; return; }
	//wait for a connection 
	//loop for connection (select)

	clientSize = sizeof(client_addr);

	// Create the master file descriptor set and zero it

	FD_ZERO(&master);

	FD_SET(listening, &master);
	//char host[NI_MAXHOST];	//client's remote name
	//char service[NI_MAXSERV];	//service (i.e. port) the client is connected on

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
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{

               	// Accept a new connection
				//SOCKET client = accept(listening, (sockaddr*)&client_addr, &clientSize);
				SOCKET client = (SOCKET)WaitForAConnection((void*)listening, clientSize);

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
				}

			}
			else // an inbound message
			{
				//inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
				getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0);
				//Handler* d = new Handler;

				//std::thread thread([sock, this] {handler.clientHandler(sock, host, master);});
				//thread.detach();
				E_CODE_MESSAGE stat = handler.clientHandler(sock, host, master);

				int errGet = err.statusHandler(stat);
			}
		}
	}

	FD_CLR(listening, &master);
	closesocket(listening);

}

void WinServer::Cleanup()
{
	const std::string msg = "Server is shutting down. Goodbye\r\n";
	while (master.fd_count > 0)
	{
		const SOCKET sock = master.fd_array[0];
		send(sock, msg.c_str(), msg.size() + 1, 0);
		FD_CLR(sock, &master);

		closesocket(sock);

	}
	WSACleanup();
}
