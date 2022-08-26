#include "IClient.h"

class CLinuxClient : public IClient
{
public:
	CLinuxClient(const std::string& ipAddress, int port);
	~CLinuxClient() override;
	bool Init() override;
	void Run() override;

private:

	int sock{};
	void Cleanup() override;
	void CreateSocket() override;
};