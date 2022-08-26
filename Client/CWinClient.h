#include "IClient.h"

class CWinClient: public IClient
{
public:
	CWinClient(std::string ipAddress, int port);
	~CWinClient();
	bool Init() override;
	void Run() override;

private:

	SOCKET sock{};
	void Cleanup() override;
	void CreateSocket() override;
};