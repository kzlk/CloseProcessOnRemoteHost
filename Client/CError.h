#pragma once
#include <iostream>

enum class E_CODE_MESSAGE
{
	E_MAP_EMPTY,
	E_SEND_H_STATUS,
	E_SEND_STATUS,
	E_RECV_PID,
	E_H_CLOSE_PROC,
	E_GET_WIN_PROC,
	E_SEND_H_LIST,
	E_SIZE_MAP,
	E_SEND_MAP,
	E_SEND_H_PID,
	E_SEND_H_NAME,
	E_SEND_PID,
	E_SEND_H_DISC,
	SUCCESS,
	FAILURE
};

typedef E_CODE_MESSAGE msg;

class CError
{
public:
	int statusHandler(const E_CODE_MESSAGE& err);
};