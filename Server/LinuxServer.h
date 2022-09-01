#ifdef __linux__
#pragma once
#include "IServer.h"
#define MAX_CLIENTS 10

class CServer: public IServer
{
public:
	CServer(const std::string& ipAddress, int port);
	~CServer() override;
	bool Init() override;
	void Run() override;
	void Cleanup()override;

private:
    int client_socket[MAX_CLIENTS]{};
	int listening{};
    bool CreateSocket() override;
    int WaitForAConnection(int listening) override;
};
#endif