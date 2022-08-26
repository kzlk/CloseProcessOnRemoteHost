#include "IClient.h"

#include <utility>

IClient::IClient(std::string ipAddress, const int port)
:m_ipAddress(std::move(ipAddress)), port(port){}

IClient::~IClient() = default;
