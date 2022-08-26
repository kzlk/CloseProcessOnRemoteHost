#include "CError.h"



int CError::statusHandler(const E_CODE_MESSAGE& err)
{
	switch (err)
	{

	case msg::E_H_CLOSE_PROC:

		std::cerr << "Error in sending header \"close proc\"" << '\n';

		return  -1;

	case msg::E_MAP_EMPTY:

		std::cerr << "List of processes is EMPTY" << '\n';
		return  -1;

	case msg::E_RECV_PID:

		std::cerr << "Error! Can not receive PID from server" << '\n';
		return  -1;

	case msg::E_SEND_H_STATUS:

		std::cerr << "Error! Could not send header \"status\"" << '\n';
		return  -1;

	case msg::E_SEND_STATUS:

		std::cerr << "Error! Could not send status of closing process" << '\n';
		return  -1;

	case msg::E_GET_WIN_PROC:
		std::cerr << "Could not get Windows processes " << '\n';
		return  -1;

	case msg::E_SEND_H_LIST:
		std::cerr << "Error in sending header for list process" << '\n';
		return  -1;

	case msg::E_SIZE_MAP:
		std::cerr << "Cannot send size of map" << '\n';
		return  -1;

	case msg::E_SEND_MAP:
		std::cerr << "Cannot send serialized map" << '\n';
		return  -1;

	case E_CODE_MESSAGE::E_SEND_H_PID:
		std::cerr << "Cannot send header \"get_pid\"" << '\n';
		return  -1;

	case E_CODE_MESSAGE::E_SEND_H_NAME:
		std::cerr << "Cannot send header \"get_name\"" << '\n';
		return  -1;

	case E_CODE_MESSAGE::E_SEND_PID:
		std::cerr << "Cannot send PID to server" << '\n';
		return  -1;

	case msg::E_SEND_H_DISC:
		std::cerr << "Cannot send disconnect header" << '\n';
		return  -1;

	case msg::FAILURE:
		return -1;

	case msg::SUCCESS:
		return 0;

	default: std::cerr << "Unknown status" << '\n'; return  -1;

	}
	
}
