#pragma once

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

#include <iostream>
#include <string>
#include <sstream>
using namespace std;


