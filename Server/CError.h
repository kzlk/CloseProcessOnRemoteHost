#pragma once
#include <iostream>

enum class E_CODE_MESSAGE
{
	E_REC_LIST_PROC,
	E_GET_SER_MAP,
	E_GET_BYTES,
	E_MAP_EMPTY,
	E_SEND_H_CLOSE,
	E_SEND_PID,
	E_IS_SUCCESS_CLOSE,
	E_IS_FAILED_CLOSE,
	E_STATUS_UNKNOWN,
	E_PID_EMPTY,
	E_GET_PID,
	E_GET_NAME,
	E_STOP_SERVER
};

typedef E_CODE_MESSAGE error;

class CError
{
public:

	//TODO: handle some status and do job 
	static int statusHandler(const E_CODE_MESSAGE& err);

};
