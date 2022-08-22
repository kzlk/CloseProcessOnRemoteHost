#include "Client.h"
#include "CSerializeMap.h"
#include "Process.h"

#define MESSAGE_HEADER_SIZE 20
#define E_RECV_SEND -1
#define HEADER 20
using namespace std;

// TODO: (22.08.2022) 
// push commit to github
// creaet


SOCKET sock;
Process proc;


struct messageHeader
{
	char listProc[MESSAGE_HEADER_SIZE] = "list_proc";
	char closeProc[MESSAGE_HEADER_SIZE] = "close_proc";
	char closeStatus[MESSAGE_HEADER_SIZE] = "status";


}header;



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
	E_CLOSE_PROC

};

typedef E_CODE_MESSAGE error;
//handling all receiveing message in new thread




auto clientHandler()
{
	/*
	char buf[4096];
	while (true)
	{
		memset(buf, 0, 4096);
		//Wait for response
		int bytesReceived = recv(sock, buf, 4096, 0);
		if (bytesReceived > 0)
		{
			//Echo response to console
			std::cout << "SERVER> " << std::string(buf, 0, bytesReceived) << '\n';
		}
	}
	*/
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

		if (strcmp(header.closeProc, headerBuf) == NULL)
		{
			//receive PID 
			if (recv(sock, (char*)&PID, sizeof(PID), NULL) == E_RECV_SEND)
				return error::E_RECV_PID;

			if (send(sock, header.closeStatus, sizeof(header.closeStatus), NULL) == E_RECV_SEND)
				return error::E_SEND_H_STATUS;

			//close process
			if (proc.closeProcessByPID(PID) != NULL)
			{
				
				if (send(sock, (char*)&isSuccess, sizeof(isSuccess), NULL) == E_RECV_SEND)
					return error::E_SEND_STATUS;
				std::cout << "Process ...... with PID" << PID << "closed successful" << '\n';

			}else 
			{
				if (send(sock, (char*)&isFailed, sizeof(isFailed), NULL) == E_RECV_SEND)
					return error::E_SEND_STATUS;
			}
				


			//TODO: send message to server 
		}
		else {

		}


	}

}

//send all data 
int sendall(int s, vector<char> buf, int* len)
{
	int total = 0; // сколько байт мы послали
	int bytesleft = *len; // сколько байт осталось послать
	int n;
	while (total < *len) {
		n = send(s, buf.data() + total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total; // здесь количество действительно посланных байт
	return n == -1 ? -1 : 0; // вернуть -1 при сбое, 0 при успехе
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

	//do-wile loop to send and receive data

	char buf[4096];
	std::string userInput;
	//Process process; // Class Process for process manupulation

	//std::thread thread(clientHandler); // thread to start clientHandler in new thread 

	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, NULL, NULL, NULL);
	int j = 1;
	std::string msg1;
	do
	{

#ifdef _WIN32
		auto serMap = proc.processNamePID();
		if (serMap.empty()) {std::cerr << "Could not get Windows processes " << '\n'; return 0;}
#endif //
		//TODO: friendly user console interface

		std::vector<char> byteStream = serMap.serialize();
		//serializable_map<int, std::string> serMap2;
		//serMap2.deserialize(byteStream);

		int len = byteStream.size();

		//send header list of processes
		int sendHeaderFor = send(sock, header.listProc, MESSAGE_HEADER_SIZE, NULL);
		if (sendHeaderFor == E_RECV_SEND) { std::cerr << "Error in sending header for list process" << '\n'; return 0; }

		//SEND TO SERVER SIZE OF MAP
		int sendSize = send(sock, (char*)&len, sizeof(int), NULL); //
		if (sendSize == -1) { std::cerr << ("Cannot send size of map") << '\n'; return 0; };


		if (sendall(sock, byteStream, &len) == -E_RECV_SEND)
		{
			std::cerr << ("Cannot send serialized map") << '\n';
			std::cout << len << " bytes sent" << '\n';
			exit(1);
		}

		auto response = clientHandler();
		/* 1) function to response to received header
		 * 2) func return status
		 * 3) handle status
		 * 4) if (status == SUCCESS) and ask do you want to continue press 1 or type /quit to exit
		 *
		 *
		 *
		 *
		 *
		 */



		//quit


		/*if (sendSize2 == -1)
		{
			memset(buf, 0, 4096);
			//Wait for response
			int bytesReceived = recv(sock, buf, 4096, 0);
			if(bytesReceived > 0)
			{
				//Echo response to console
				std::cout << "SERVER> " << std::string(buf, 0, bytesReceived) << '\n';
			}


			perror("sendall");
			printf("We only sent %d bytes because of the error!\n", len);

			//}

		//}
		}
	*/


		system("pause"); // wait for user response

	} while (true);
	//thread.join();


	//gracefully close down everything
	closesocket(sock);
	WSACleanup();


	return 0;
}


