#pragma once
#ifdef _WIN32
#pragma once
#include "IClient.h"


class CClient: public IClient
{
public:
	CClient(std::string ipAddress, int port);
	~CClient() override;
	bool Init() override;
	void Run() override;

private:

	SOCKET sock{};
	void Cleanup() override;
	bool CreateSocket() override;
};
#endif