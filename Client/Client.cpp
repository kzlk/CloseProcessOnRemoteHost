﻿#include "Client.h"
#include "CSerializeMap.h"
#include "Process.h"
//TODO: PORT FOR LINUX

#define MESSAGE_HEADER_SIZE 20
#define E_RECV_SEND (-1)
using namespace std;

SOCKET sock;
Process proc;

//send all data 
int sendall(const int s, const vector<char>& buf, int* len)
{
	int total = 0; // how many bytes did we send
	int bytesleft = *len; // how many bytes are left to send
	int n = 0;
	while (total < *len) {
		n = send(s, buf.data() + total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total; // here is the number of bytes actually sent
	return n == -1 ? -1 : 0; //  -1 when error, 0 when success
}

//header for message
struct messageHeader
{
	char listProc[MESSAGE_HEADER_SIZE]    = "list_proc";
	char closeProc[MESSAGE_HEADER_SIZE]   = "close_proc";
	char closeStatus[MESSAGE_HEADER_SIZE] = "status";
	char getListProc[MESSAGE_HEADER_SIZE] = "get_list_proc";
	char getPID[MESSAGE_HEADER_SIZE]      = "get_pid";
	char getName[MESSAGE_HEADER_SIZE]     = "get_name";
	char selProc[MESSAGE_HEADER_SIZE]     = "sel_proc";

}header;

//TODO: make class for error handling
//TODO: enum for status of closing process
enum class E_CODE_MESSAGE
{
	E_CLOSED_SUCCES,
	E_REC_LIST_PROC,
	E_GET_SER_MAP,
	E_GET_BYTES,
	E_MAP_EMPTY,
	E_SEND_H_STATUS,
	E_SEND_STATUS,
	E_RECV_PID,
	E_H_CLOSE_PROC,
	E_CLOSE_PROC,
	E_GET_WIN_PROC,
	E_SEND_H_LIST,
	E_SIZE_MAP,
	E_SEND_MAP, 
	E_SEND_H_PID,
	E_SEND_H_NAME, 
	E_SEND_PID,
	E_SEND_NAME
};

typedef E_CODE_MESSAGE error;

//handle receiving message
//TODO: exit from cycle
//TODO: output name of process by received PID
auto clientHandler()
{

	int PID{};
	int isSuccess = 0;
	int isFailed = -1;
	char headerBuf[MESSAGE_HEADER_SIZE]{};
	ZeroMemory(headerBuf, MESSAGE_HEADER_SIZE);

	int msg_size;
	while (true)
	{
		//receive header
		if (recv(sock, headerBuf, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
			return error::E_H_CLOSE_PROC;

		//received header to close process
		if (strcmp(header.closeProc, headerBuf) == NULL)
		{
			//receive PID 
			if (recv(sock, (char*)&PID, sizeof(PID), NULL) == E_RECV_SEND)
				return error::E_RECV_PID;
			std::cout << " Server > : received PID " << PID << '\n';

			//send closestatus header 
			if (send(sock, header.closeStatus, sizeof(header.closeStatus), NULL) == E_RECV_SEND)
				return error::E_SEND_H_STATUS;

			//close process
			if (proc.closeProcessByPID(PID) != NULL)
			{
				//send status is success
				if (send(sock, (char*)&isSuccess, sizeof(isSuccess), NULL) == E_RECV_SEND)
					return error::E_SEND_STATUS;
				std::cout << " > Process ...... with PID " << PID << " closed successful" << '\n';

			}else 
			{
				//send status is failure
				if (send(sock, (char*)&isFailed, sizeof(isFailed), NULL) == E_RECV_SEND)
					return error::E_SEND_STATUS;
				std::cout << " > Process ...... with PID " << PID << " close failed" << '\n';
			}

		}
		else if (strcmp(header.getListProc, headerBuf) == NULL) 
		{
#ifdef _WIN32
			auto serMap = proc.processNamePID();
			if (serMap.empty()) { return error::E_GET_WIN_PROC;}
#endif //
			//TODO: friendly user console interface

			std::vector<char> byteStream = serMap.serialize();
			//serializable_map<int, std::string> serMap2;
			//serMap2.deserialize(byteStream);
			int len = byteStream.size();

			//send header list of processes
			int sendHeaderFor = send(sock, header.listProc, MESSAGE_HEADER_SIZE, NULL);
			if (sendHeaderFor == E_RECV_SEND) { return error::E_SEND_H_LIST; }

			//send to server size of map(processes list)
			int sendSize = send(sock, (char*)&len, sizeof(int), NULL); //
			if (sendSize == -1) {  return error::E_SIZE_MAP; }

			//send serialized map to server
			if (sendall(sock, byteStream, &len) == E_RECV_SEND)
			{
				std::cout << "Serialized map --> " << len << " bytes sent" << '\n';
				return error::E_SEND_MAP;
			}

			//enter PID or name of process
			int choice {};
			int enterPID{};
			string enterName{};
			std::cout << "How do you want to close the process?" << '\n';
			std::cout << "1) By PID : enter -> 1 \n2) By name : enter -> 2\n3) Exit: enter -> 3\n";
			
			while (choice != 2 && choice != 1)
			{
				std::cin >> choice;
				switch (choice)
				{
				case 1:
					std::cout << "Please enter process' PID: ";
					std::cin >> enterPID;

					//send header that we enter PID
					if (send(sock, header.getPID, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
						return error::E_SEND_H_PID;

					//send input PID
					if (send(sock, (char*)&enterPID, sizeof(int), NULL) == E_RECV_SEND)
						return error::E_SEND_PID;

					//do some job
					break;
				case 2:
					std::cout << "Please enter process' name : ";
					std::cin >> enterName;

					//send header that we enter NAME
					if (send(sock, header.getName, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
						return error::E_SEND_H_PID;

					//send input NAME
					if (send(sock, enterName.c_str(), enterName.size() + 1, NULL) == E_RECV_SEND)
						return error::E_SEND_PID;

					//do some job
					break;
				default:
					std::cout << "Wrong input! Please try again!\n Enter your choice ";
					break;
				}
			}

			//send header to select process
			if (send(sock, header.selProc, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
				return error::E_SEND_H_PID;
			
		}

	}

}

//handle 
auto statusHandler(const E_CODE_MESSAGE& err)
{
	switch(err)
	{
	case error::E_CLOSED_SUCCES :

		
		break;

	case error::E_CLOSE_PROC:


		break;

	case error::E_GET_BYTES:

		break;

	case error::E_GET_SER_MAP:

		break;

	case error::E_H_CLOSE_PROC:


		break;

	case error::E_MAP_EMPTY:


		break;

	case error::E_RECV_PID:

		break;

	case error::E_REC_LIST_PROC:


		break;
	case error::E_SEND_H_STATUS:


		break;
	case error::E_SEND_STATUS:

		break;

	case error::E_GET_WIN_PROC:
		std::cerr << "Could not get Windows processes " << '\n';
		break;

	case error::E_SEND_H_LIST:
		std::cerr << "Error in sending header for list process" << '\n';
		break;

	case error::E_SIZE_MAP:
		std::cerr << "Cannot send size of map" << '\n';
		break;

	case error::E_SEND_MAP:
		std::cerr << "Cannot send serialized map" << '\n';
		break;
	}

	
		
	return 0;
}

int main()
{
	//start menu

	std::string ipAddress{};
	unsigned port{};

	std::cout << "Enter IP Address of the Server that you want to connect: ";

	std::cin >> ipAddress;

	std::cout << "Enter Server's listening Port that you want to connect: ";

	std::cin >> port;

	//initialize winsock

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		std::cerr << "Can't Initialize winsock!, error #" << WSAStartup(DLLVersion, &wsaData) << '\n';
		exit(1);
	}

	//create socket - identifier that our program will use 
	//to communicate with the network layer
	//to send data uot to the remote server

	sock = socket(AF_INET, SOCK_STREAM, NULL);

	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Cannot create a socket! Errror #" << WSAGetLastError() << '\n';
		WSACleanup();
		exit(1);
	}

	//fill in a hint structure

	sockaddr_in hint{};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//connect to server

	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));

	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err # " << WSAGetLastError() << '\n';
		closesocket(sock);
		WSACleanup();
		exit(1);
	}
	std::cout << "You successfully connect to server!" << '\n';

	char buf[4096];
	std::string userInput;

	//std::thread thread(clientHandler); // thread to start clientHandler in new thread 
	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, NULL, NULL, NULL);

	bool isRepeat = true;
	
	std::string msg1{};

	//TODO: exit from cycle

	do
	{

		//handle receiving messages
		auto response = clientHandler();
		/* 1) function to response to received header
		 * 2) func return status
		 * 3) handle status
		 * 4) if (status == SUCCESS) and ask do you want to continue press 1 or type /quit to exit
		 *
		 *
		 *
		 *if(response == 1 ) then while loop else stop while loop
		 *
		 */

		const auto handle = statusHandler(response);
		if(handle == 1)
		{
			// repeat = true
		}
		else
		{
			//repeat = false
		}

		//quit

		system("pause"); // wait for user response

	} while (isRepeat);
	//thread.join();

	//gracefully close down everything
	closesocket(sock);
	WSACleanup();


	return 0;
}


