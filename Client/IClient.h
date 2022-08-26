#include "CClientHandler.h"

class IClient
{
public:
	IClient(std::string ipAddress, int port);
	virtual ~IClient();
	virtual bool Init() = 0;
	virtual void CreateSocket() = 0;
	virtual void Run() = 0;
	virtual void Cleanup() = 0;
protected:
	CError catchy;
	CClientHandler handler;
	std::string m_ipAddress;
	int port;
	bool isRepeat = true;
};