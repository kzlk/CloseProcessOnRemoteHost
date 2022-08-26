#pragma once
#include "IServer.h"

class WinServer: public IServer 
{
public:
	WinServer(std::string ipAddress, int port);
	~WinServer() override;
	bool Init() override;
	void Run() override;
private:
	messageHeader header;
	SOCKET listening{};
	void* CreateSocket() override;
	void* WaitForAConnection(void *listening, int& clientSiz) override;
	void Cleanup()override;

};