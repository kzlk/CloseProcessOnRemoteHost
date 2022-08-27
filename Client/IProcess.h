#pragma once
#include "CSerializeMap.h"

class IProcess
{
public:
	virtual ~IProcess() = default;
	virtual serializable_map<int, std::string> processNamePID() = 0;
	virtual int closeProcessByPID(int PID) = 0;
protected:
	serializable_map<int, std::string> map_;
};
