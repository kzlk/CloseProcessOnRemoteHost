#include "IServer.h"

IServer::IServer(std::string ipAddress, const int port) : handler(), m_ipAddress(std::move(ipAddress)), port(port)
{
}

IServer::~IServer() = default;
