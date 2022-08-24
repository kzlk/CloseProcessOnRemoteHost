#include "Server.h"
#define MESSAGE_HEADER_SIZE 20
#define E_RECV_SEND (-1)

//TODO: PORT FOR LINUX

//TODO: logic if it is a system process
//func to select process will be close
auto selProcessToClose(const serializable_map<int, std::string>& map, const std::string& process)
{

	//TODO: read from file win_sys_proc.txt
	//TODO: send to client that it is a system process 
#ifdef _WIN32
	std::string sysProc[] = { "svchost.exe", "lsass.exe" , "explorer.exe", "sihost.exe",
		"ntoskrnl.exe", "system",
		"wininit.exe" , " dwm.exe" , "smss.exe", "services.exe",
		"winlogon.exe", "csrss.exe" ,
		"RuntimeBroker.exe", "ApplicationFrameHost.exe",
		"ctfmon.exe", "SecurityHealthService.exe", "spoolsv.exe" };
	//const std::string process = "chrome.exe"; //hardcode for testing
#endif
	//TODO: linux lin_sys_proc.txt

	//TODO: logic for chosing the process to close

	std::vector<int> bufId;
	for (const auto& entry : map)
	{

		for (auto& it : sysProc)
		{
			if (entry.second == it)
			{
				//TODO: logic if it a system process
				continue;
			}

			if (entry.second == process)
			{
				bufId.emplace_back(entry.first);
			}
		}

	}

	if (!bufId.empty())
	{
		auto it = bufId.begin();
		if (bufId.size() == 1) return *it;
		if (bufId.size() > 1) { std::advance(it, std::rand() % bufId.size()); return *it; };
	}

	return -1; //process is not exist
};

//TODO: logic if it is a system process 
auto selProcessToClose(const serializable_map<int, std::string>& map, const int& PID )
{
	if (map.find(PID) != map.end())
		return PID;
	return -1; // PID in not exist 
}

//header for message
 struct messageHeader
{
	 char listProc[MESSAGE_HEADER_SIZE]    =  "list_proc";
	 char closeProc[MESSAGE_HEADER_SIZE]   =  "close_proc";
	 char closeStatus[MESSAGE_HEADER_SIZE] =  "status";
	 char getListProc[MESSAGE_HEADER_SIZE] =  "get_list_proc";
	 char getPID[MESSAGE_HEADER_SIZE]      =  "get_pid";
	 char getName[MESSAGE_HEADER_SIZE]     =  "get_name";
	 char selProc[MESSAGE_HEADER_SIZE]     = "sel_proc";
}header;

 //TODO: error handler
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

//TODO: func which start in new thread and recv header from client and do some job
auto clientHandler(const SOCKET &sock, char* buffer) 
{
	char headerBuf[MESSAGE_HEADER_SIZE];
	ZeroMemory(headerBuf, MESSAGE_HEADER_SIZE); // Windows

	unsigned sizeStreamBytes{};
	serializable_map<int, std::string> serMap{};
	std::vector<char> recvDataByte{};
	static int PID;
	char buf[4096];
	static int getPID{}; //input from user PID
	static std::string getName{}; //input from user names
	static bool isPIDSent{};

	while (true) {

		int isGetListProc = recv(sock, headerBuf, MESSAGE_HEADER_SIZE, NULL);
		if (isGetListProc == E_RECV_SEND) { return error::E_REC_LIST_PROC; };


		if (strcmp(headerBuf, header.listProc) == NULL)
		{
			// Receive num of bytes in map<> from client
			int bytesIn = recv(sock, (char*)&sizeStreamBytes, sizeof(sizeStreamBytes), 0);
			if (bytesIn == E_RECV_SEND) { return error::E_GET_BYTES; };

			//buf to store stream of bytes
			recvDataByte.resize(sizeStreamBytes);

			//recieve serialized map data
			int bytesStream = recv(sock, ((char*)recvDataByte.data()), sizeStreamBytes, 0);
			if (bytesStream == E_RECV_SEND) { return error::E_GET_SER_MAP; };


			//deserialize received map
			serMap.deserialize(recvDataByte);
			if (serMap.empty()) { return error::E_MAP_EMPTY; }

			std::cout << "> Received list of process from " << buffer << " (SOCKET# " << sock << ")" << '\n';


		}
		else if (strcmp(headerBuf, header.closeStatus) == NULL)
		{
			//receive closing status

			int codeStatus{};
			int isSuccess = 0;
			int isFailed = -1;

			int status = recv(sock, (char*)&codeStatus, sizeof(codeStatus), NULL);
			if (status != E_RECV_SEND)
			{
				if (codeStatus == isSuccess)
				{
					std::cout << "> User: " << buffer << " (SOCKET# " << sock << ")" << "successfully closed process with PID " << PID << '\n';
					return error::E_IS_SUCCESS_CLOSE;
				}
				if (codeStatus == isFailed)
				{
					std::cout << "> User: " << buffer << " (SOCKET# " << sock << ")" << "could not closed process with PID " << PID << '\n';
					return error::E_IS_FAILED_CLOSE;

				}
				return error::E_STATUS_UNKNOWN;
			}

		}
		else if(strcmp(header.getPID, headerBuf) == NULL)
		{
			//recieve PID
			if (recv(sock, (char*)&getPID, sizeof(int), NULL) == E_RECV_SEND)
			{
				isPIDSent = true;
				return error::E_GET_PID;
			}
				
		}
		else if(strcmp(header.getName, headerBuf) == NULL)
		{
			ZeroMemory(buf, 4096);
			//received name
			const int bytesReceived = recv(sock, (char*)&buf, 4096, NULL);
			if ( bytesReceived != E_RECV_SEND)
			{
				getName = string(buf, 0, bytesReceived);
				isPIDSent = false;
				
			}else
			{
				return error::E_GET_PID;
			}
				
		}else if(strcmp(header.selProc, headerBuf) == NULL)
		{
			//sel process to close

			if(isPIDSent)
			{
				//return sel proc by PID
				PID = selProcessToClose(serMap, getPID);
			}else
			{
				//return sel proc by name
				PID = selProcessToClose(serMap, getName);
			}

			if (PID == E_RECV_SEND) { return error::E_PID_EMPTY; } //logic if proc not exist


			//send header to process close
			if (send(sock, header.closeProc, sizeof(header.closeProc), NULL) == E_RECV_SEND)
				return error::E_SEND_H_CLOSE;

			//send PID process to close
			if (send(sock, (char*)&PID, sizeof(PID), NULL) == E_RECV_SEND)
				return error::E_SEND_PID;


		}
		//TODO: send that is end of execution

	}

	//TODO: drop client
	/*
	if (bytesIn <= 0)
	{
		// Drop the client
		closesocket(sock);
		FD_CLR(sock, &master);
	}
	*/

}

//TODO: handle some status and do job 
auto statusHandler(E_CODE_MESSAGE &err)
{
	//switch - case
};


void main()
{
#ifdef _WIN32
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}
#endif;

	// Bind the ip address and port to a socket
	sockaddr_in hint{};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	//wait for a connection 
	sockaddr_in client_addr;
	int clientSize = sizeof(client_addr);

	// Create the master file descriptor set and zero it
	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);
	char host[NI_MAXHOST];//client's remote name
	char service[NI_MAXSERV]; //service (i.e. port) the client is connected on

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);
	bool running = true;

	while (running)
	{

		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, (sockaddr*)&client_addr, &clientSize);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);


				if (getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
				{
					std::cout << host << " connected on port " << service << endl;
				}
				else
				{
					inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
					std::cout << host << " connected on port " << ntohs(client_addr.sin_port) << '\n';
				}

				//send request to get list of processes from client
				send(client, header.getListProc, MESSAGE_HEADER_SIZE, NULL);
			}
			else // an inbound message
			{
				//inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
				getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0);
				
				auto stat = clientHandler(sock, host);

				statusHandler(stat);
			}
		}
	}

	FD_CLR(listening, &master);
	closesocket(listening);

	string msg = "Server is shutting down. Goodbye\r\n";

	while (master.fd_count > 0)
	{
		SOCKET sock = master.fd_array[0];
		send(sock, msg.c_str(), msg.size() + 1, 0);
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();

	//system("pause");
}