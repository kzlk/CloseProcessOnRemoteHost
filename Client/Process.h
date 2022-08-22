#pragma once
#include "Client.h"
#include <tlhelp32.h> 
#include <string>
#include <tchar.h>
#include <windows.h>
#include "CSerializeMap.h"
#include <tlhelp32.h>

class Process
{
private:
	HANDLE hProcessSnap; //create handle identifier for object hprocesssnap
	HANDLE hProcess; //create handle identifier for object hprocess
	PROCESSENTRY32 pe32; //Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
	DWORD dwPriorityClass; //create dword class dwPriorityClass
	serializable_map<int, std::string> map;
	
public:
	serializable_map<int, std::string> processNamePID();
	int closeProcessByPID(int PID);
};

