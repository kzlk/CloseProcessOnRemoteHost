#include "Process.h"


#ifdef _WIN32

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


		map_.insert(std::pair<int, std::string>(pe32.th32ProcessID, str));

	};

	return map_;
}

#elif defined(__linux__)

int Process::closeProcessByPID(int PID)
{
    return kill(PID, SIGSEGV);
}

serializable_map<int, std::string> Process::processNamePID()
{

    dir = opendir(PROC_DIR);
    if (dir == nullptr)
    {
        printf("opendir() failed with error %d\n", errno);
        return map_;
    }

    while ((de = readdir(dir)))
    {
        pid = de->d_name;
        if (!isnumber(pid)) {
            continue;
        }
        char exelinkpath[PATH_MAX];
        snprintf(exelinkpath, sizeof(exelinkpath), "%s%s%s", PROC_DIR, pid, EXE_LINK);

        if (stat(exelinkpath, &st) == -1)
        {
            // Either the file doesn't exist or it's not accessible.
            continue;
        }
        char path[PATH_MAX];
        if (!getlinkedpath(exelinkpath, path))
        {
            printf("getlinkedpath() failed, skipping entry...\n");
            continue;
        }
        map_.insert(std::pair<int, std::string >(atoi(pid), basename(path)));
    }
    closedir(dir);
    return map_;
}

bool Process::isnumber(const char* string)
{

    while (string[0] != '\0') {
        if (!isdigit(string[0])) {
            return false;
        }

        ++string;
    }

    return true;
}

bool Process::getlinkedpath(const char* linkpath, char* linkedpath)
{

    if (!realpath(linkpath, linkedpath)) {
        printf("realpath() failed with error %d\n", errno);
        return false;
    }

    return true;

}

#else
#endif
