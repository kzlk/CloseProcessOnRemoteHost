#include "IProcess.h"

#ifdef __linux__
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <dirent.h>
#include <cerrno>
#include <libgen.h>
#include <linux/limits.h>
#include <sys/stat.h>
#define ERROR_CLOSE -1
#define PROC_DIR "/proc/"
#define EXE_LINK "/exe"

class CProcess : public IProcess
{

public:
	serializable_map<int, std::string> processNamePID() override;
	int closeProcessByPID(int PID) override;

private:
	bool isnumber(const char* string);
	bool getlinkedpath(const char* linkpath, char* linkedpath);
	DIR* dir = nullptr;
	struct dirent* de{};
	const char* pid = nullptr;
};


#endif