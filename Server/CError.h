#pragma once

enum class E_CODE_MESSAGE
{
	E_CLOSED_SUCCES,
	E_REC_LIST_PROC,
	E_GET_SER_MAP,
	E_GET_BYTES,
	E_MAP_EMPTY,
	E_SEND_H_CLOSE,
	E_SEND_PID,
	E_CLOSE_PROC,
	E_IS_SUCCESS_CLOSE,
	E_IS_FAILED_CLOSE,
	E_STATUS_UNKNOWN,
	E_PID_EMPTY,
	E_GET_PID,
	E_GET_NAME,
	E_GET_H_SEL_PROC
};

typedef E_CODE_MESSAGE error;

class CError
{
public:

	//TODO: handle some status and do job 
	auto statusHandler(const E_CODE_MESSAGE& err)
	{
		//switch - case
		switch (err)
		{
		case error::E_CLOSED_SUCCES:

			break;
		}
	};


};
