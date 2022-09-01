#include "IClient.h"

class CClient : public IClient
{
public:
	CClient(const std::string& ipAddress, int port);
	~CClient() override;
	bool Init() override;
	void Run() override;

private:

	int sock{};
	void Cleanup() override;
	bool CreateSocket() override;
};