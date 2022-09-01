
#include "CLinuxProcess.h"

#ifdef __linux__
serializable_map<int, std::string> CProcess::processNamePID()
{
    struct stat st{};

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

int CProcess::closeProcessByPID(int PID)
{
	return kill(PID, SIGSEGV);
}

bool CProcess::isnumber(const char* string)
{
    while (string[0] != '\0') {
        if (!isdigit(string[0])) {
            return false;
        }

        ++string;
    }

    return true;

}

bool CProcess::getlinkedpath(const char* linkpath, char* linkedpath)
{
    if (!realpath(linkpath, linkedpath)) {
        printf("realpath() failed with error %d\n", errno);
        return false;
    }

    return true;

}

#endif