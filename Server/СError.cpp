#pragma once

#include <iostream>

#include "CError.h"

/*
* @return -1 when critical error
* @return  1 when server is stopping
* @return  0 when process successful closed
 */

int CError::statusHandler(const E_CODE_MESSAGE& err)
{

	switch (err)
	{
	case E_CODE_MESSAGE::E_REC_LIST_PROC:
		std::cerr << "Could not receive header to header buffer\n";
		break;
	case E_CODE_MESSAGE::E_GET_SER_MAP: 
		std::cerr << "Could not receive serializes map\n";
		break;
	case E_CODE_MESSAGE::E_GET_BYTES: 
		std::cerr << "Could not receive count of bytes in map\n";
		break;
	case E_CODE_MESSAGE::E_MAP_EMPTY: 
		std::cerr << "Map is empty\n";
		break;
	case E_CODE_MESSAGE::E_SEND_H_CLOSE:
		std::cerr << "Could not sent header \"close_proc\"\n";
		break;
	case E_CODE_MESSAGE::E_SEND_PID:
		std::cerr << "Could not sent PID to client\n";
		break;
	case E_CODE_MESSAGE::E_IS_SUCCESS_CLOSE:
		return 0;
		
	case E_CODE_MESSAGE::E_IS_FAILED_CLOSE:
		break;
	case E_CODE_MESSAGE::E_STATUS_UNKNOWN:
		std::cerr << "Unknown status of process closing\n";
		break;
	case E_CODE_MESSAGE::E_PID_EMPTY: 
		std::cerr << "Does not find process in received list\n";
		break;
	case E_CODE_MESSAGE::E_GET_PID: 
		std::cerr << "Error getting PID\n";
		
		break;
	case E_CODE_MESSAGE::E_GET_NAME: 
		std::cerr << "Error getting NAME of process\n";
		break;

	case E_CODE_MESSAGE::E_STOP_SERVER:
		return 1;
	default: std::cerr << "Unknown status of process closing\n"; break; ;
	}
	return -1;

	
}
