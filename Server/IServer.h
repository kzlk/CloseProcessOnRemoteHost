#pragma once 
#include "CServerHandler.h"
#include <string>


class IServer
{
public:
	IServer(std::string ipAddress, int port); 
	virtual ~IServer(); 
	virtual bool Init() = 0;
	virtual void* CreateSocket() = 0;
	virtual void* WaitForAConnection( void *listening, int& clientSiz) = 0;
	virtual void Run() = 0;
	virtual void Cleanup() = 0;

protected:
	CError err;
	Handler handler;
	std::string m_ipAddress;
	int port;

	int clientSize{};
	sockaddr_in client_addr{};
	fd_set master{};

	char host[NI_MAXHOST]{};	//client's remote name
	char service[NI_MAXSERV]{};	//service (i.e. port) the client is connected on
};