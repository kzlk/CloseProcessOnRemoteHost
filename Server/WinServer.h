#ifdef _WIN32
#pragma once
#include "IServer.h"

class WinServer final : public IServer 
{
public:
	WinServer(std::string ipAddress, int port);
	~WinServer() override;
	bool Init() override;
	void Run() override;
private:


	SOCKET listening{};
	SOCKET CreateSocket() override;
	SOCKET WaitForAConnection(SOCKET listening) override;
	void Cleanup() override;

};
#endif