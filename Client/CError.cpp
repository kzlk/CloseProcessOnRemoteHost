#include "CError.h"



int CError::statusHandler(const E_CODE_MESSAGE& err)
{

	/*
	 *@ return -1 when error is critical
	 *@ return 1  when server shutdown
	 *@ return 0  when all success
	 */

	switch (err)
	{

	case msg::E_H_CLOSE_PROC:

		std::cerr << "Error in receiving header \"close proc\"" << '\n';

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

	case msg::E_RECV_EXIT:
		std::cerr << "Cannot receive message from closing server" << '\n';
		return  -1;

	case msg::SHUTDOWN_CLIENT:
		return 1;

	default: std::cerr << "Unknown status" << '\n'; return  -1;

	}
	
}
