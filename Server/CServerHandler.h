#pragma once
#include "CError.h"
#include "CSerializeMap.h"
#include <string>
#include <iostream>

#ifdef __linux__
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#elif defined(_WIN32)
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#else
#endif

#define MESSAGE_HEADER_SIZE 20
#define E_RECV_SEND (-1)

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


class Handler
{
	char headerBuf[MESSAGE_HEADER_SIZE] = {}; //save received header
	unsigned sizeStreamBytes{};
	serializable_map<int, std::string> serMap{};
	std::vector<char> recvDataByte{};
	char buf[4096] = {};
	
public:
	messageHeader header;
	E_CODE_MESSAGE clientHandler(const SOCKET& sock, const char* buffer, fd_set& master);
	int selProcessToClose(const serializable_map<int, std::string>& map, const std::string& process);
	int selProcessToClose(const serializable_map<int, std::string>& map, const int& PID);
};


