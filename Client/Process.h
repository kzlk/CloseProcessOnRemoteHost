#pragma once

#ifdef _WIN32
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#include <tlhelp32.h> 
#include <windows.h>


#elif defined (__linux__)

#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <libgen.h>
#include <linux/limits.h>
#include <map>
#include <string>
#include <sys/stat.h>

#define PROC_DIR "/proc/"
#define EXE_LINK "/exe"


#else
#endif

#include <iostream>
#include <string>
#include "CSerializeMap.h"

class Process
{
#ifdef _WIN32
	HANDLE hProcessSnap = nullptr; //create handle identifier for object hprocesssnap
	HANDLE hProcess = nullptr; //create handle identifier for object hprocess
	PROCESSENTRY32 pe32 = {}; //Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.

public:

	serializable_map<int, std::string> processNamePID();
	int closeProcessByPID(int PID);

#elif defined (__linux__)
private:
	bool isnumber(const char* string);
	bool getlinkedpath(const char* linkpath, char* linkedpath);
	DIR* dir = nullptr;
	struct dirent* de{};
	const char* pid = nullptr;
public:
	serializable_map<int, std::string> processNamePID();
	int closeProcessByPID(int PID);
#else
#endif

private:
	serializable_map<int, std::string> map_;
};

/*
 * interface process 
 * class linuxProcess : public process
 * class winProcess   : public process 
 *
 */