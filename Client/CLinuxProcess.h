#ifdef __linux__

class CLinuxProcess : IProcess
{

public:
	serializable_map<int, std::string> processNamePID();
	int closeProcessByPID(int PID);

private:
	bool isnumber(const char* string);
	bool getlinkedpath(const char* linkpath, char* linkedpath);
	DIR* dir = nullptr;
	struct dirent* de{};
	const char* pid = nullptr;
};


#endif