#pragma once
#include "CError.h"
#include <string>

#ifdef __linux__
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "CLinuxProcess.h"
#define SOCKET int

#elif defined(_WIN32)
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

#include "CWinProcess.h"
#else
#endif


#define MESSAGE_HEADER_SIZE 20
#define E_RECV_SEND (-1)


//header for message
struct messageHeader
{
	char listProc[MESSAGE_HEADER_SIZE] = "list_proc";
	char closeProc[MESSAGE_HEADER_SIZE] = "close_proc";
	char closeStatus[MESSAGE_HEADER_SIZE] = "status";
	char getListProc[MESSAGE_HEADER_SIZE] = "get_list_proc";
	char getPID[MESSAGE_HEADER_SIZE] = "get_pid";
	char getName[MESSAGE_HEADER_SIZE] = "get_name";
	char selProc[MESSAGE_HEADER_SIZE] = "sel_proc";
	char disconnect[MESSAGE_HEADER_SIZE] = "disconnect";
	char closeServer[MESSAGE_HEADER_SIZE] = "goodbye";
};

class CClientHandler
{
public:
	int iDisconect(const SOCKET& sock);
	E_CODE_MESSAGE clientHandler(const SOCKET& sock);
	int sendall(int s, const std::vector<char>& buf, int* len);

private:
	serializable_map<int, std::string> serMap;
	messageHeader header;

#ifdef _WIN32
	IProcess* proc = new CWinProcess;
#elif defined (__linux__)
	IProcess* proc = new CLinuxProcess;
#endif


};