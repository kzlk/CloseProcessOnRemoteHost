#ifdef _WIN32
#pragma once
#include "IServer.h"

class CServer final : public IServer 
{
public:
	CServer(std::string ipAddress, int port);
	~CServer() override;
	bool Init() override;
	void Run() override;
	void Cleanup() override;

private:
	SOCKET listening{};
	SOCKET WaitForAConnection(SOCKET listening) override;
	bool CreateSocket() override;

};
#endif