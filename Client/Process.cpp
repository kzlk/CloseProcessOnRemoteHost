#include "Process.h"

int Process::closeProcessByPID(int PID)
{
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
	if (hProcess == NULL)
	{
		std::cout << "Unable to get handle of process: " << PID;
		std::cout << "Error is: " << GetLastError();
	}
	return TerminateProcess(hProcess, 0);
}


serializable_map<int, std::string> Process::processNamePID()
{
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //take "snapshot" of processes running
	pe32.dwSize = sizeof(PROCESSENTRY32); //size of PROCESSENTRY32 [unused]

	// record all detetected running processes #######################################
	while (Process32Next(hProcessSnap, &pe32))
	{
		//for each object (pe32) in hprocesssnap [Process32Next runs through processes]

		std::string ws(pe32.szExeFile);
		std::string str(ws.begin(), ws.end());


		map.insert(std::pair<int, std::string>(pe32.th32ProcessID, str));

	};

	return map;
}

