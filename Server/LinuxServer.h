#ifdef __linux__
#pragma once
#include "IServer.h"
#define MAX_CLIENTS 10
class LinuxServer: public IServer
{
public:
	LinuxServer(const std::string& ipAddress, int port);
	~LinuxServer() override;
	bool Init() override;
	void Run() override;
private:

    int client_socket[MAX_CLIENTS]{};
	int listening{};
    int CreateSocket() override;
    int WaitForAConnection(int listening) override;
	void Cleanup()override;
};
#endif