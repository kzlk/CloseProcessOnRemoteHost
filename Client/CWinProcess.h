#pragma once
#ifdef _WIN32
#pragma once
#include "IProcess.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#include <tlhelp32.h> 
#include <windows.h>
#include <string>
#define ERROR_CLOSE 0

class CProcess : public IProcess
{

public:
	serializable_map<int, std::string> processNamePID() override;
	int closeProcessByPID(int PID) override;

private:
	HANDLE hProcessSnap = nullptr; //create handle identifier for object hprocesssnap
	HANDLE hProcess = nullptr;    //create handle identifier for object hprocess
	PROCESSENTRY32 pe32 = {};    //Describes an entry from a list of the processes residing
	                            //in the system address space when a snapshot was taken.
};

#endif