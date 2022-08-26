#pragma once
#include "IServer.h"

class LinuxServer: public IServer
{
public:
	
	LinuxServer(std::string ipAddress, int port);
	~LinuxServer() override;
	bool Init() override;
	void Run() override;
private:
	int listening;
	void* CreateSocket() override;
	void* WaitForAConnection(void* listening, int& clientSiz) override;
	void Cleanup()override;

};