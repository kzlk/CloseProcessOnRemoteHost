#include "CServerHandler.h"

int Handler::continueWork(const SOCKET& sock) 
{
	memset(headerBuf, 0, MESSAGE_HEADER_SIZE);
	if( recv(sock, headerBuf, MESSAGE_HEADER_SIZE, NULL) != -1)
	{
		if (strcmp(headerBuf, header.continueWork) == NULL)
		{
			return 0;
		}
		if (strcmp(header.disconnect, headerBuf) == NULL)
		{

#ifdef _WIN32
			closesocket(sock);
#else defined(__linux__)
			close(sock);
#endif

			return 1;

		}
		std::cerr << "Unknown header receive" << headerBuf << "\n";
	}
	std::cerr << "can not to receive message in continue method" << '\n';
	return -1;
}

bool Handler::isGetListProc(const SOCKET& sock) const
{
	if (send(sock, header.getListProc, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
		return false;
	return true;
}


E_CODE_MESSAGE Handler::clientHandler(const SOCKET& sock, const char* buffer, fd_set& master)
{
	static int getPID; //input from user PID
	static std::string getName; //input from user names
	static bool isPIDSent;
	static int PID;

	memset(headerBuf, 0, MESSAGE_HEADER_SIZE);

	while (true) 
	{
		int isGetListProc = recv(sock, headerBuf, MESSAGE_HEADER_SIZE, NULL);
		if (isGetListProc == E_RECV_SEND) { return error::E_REC_LIST_PROC; }

		//if we get list of processes
		if (strcmp(headerBuf, header.listProc) == NULL)
		{
			// Receive num of bytes in map<> from client
			int bytesIn = recv(sock, (char*)&sizeStreamBytes, sizeof(sizeStreamBytes), 0);
			if (bytesIn == E_RECV_SEND) { return error::E_GET_BYTES; }

			//buf to store stream of bytes
			recvDataByte.resize(sizeStreamBytes);

			//recieve serialized map data
			int bytesStream = recv(sock, ((char*)recvDataByte.data()), sizeStreamBytes, 0);
			if (bytesStream == E_RECV_SEND) { return error::E_GET_SER_MAP; }


			//deserialize received map
			serMap.deserialize(recvDataByte);
			if (serMap.empty()) { return error::E_MAP_EMPTY; }

			std::cout << "> Received list of process from " << buffer << " (SOCKET# " << sock << ")" << '\n';

			/*
			 * user input for process
			 * get process id
			 * send PID to client
			 */

			int choice{};
			int enterPID{};
			std::string enterName{};
		

			while (PID != -1 && (choice != 2 && choice != 1 && choice != 4))
			{
				std::cout << "How do you want to do?" << '\n';
				std::cout << "1) Close process by PID : enter -> 1"
					"\n2) Close process by name : enter -> 2"
					"\n3) Show processes: enter -> 3"
					"\n4) Exit: enter -> 4\n";
				std::cout << "Enter your choice > ";
				std::cin >> choice;
				switch (choice)
				{
				case 1:
					std::cout << "Please enter process' PID:\n> ";
					std::cin >> enterPID;
					PID = selProcessToClose(serMap, enterPID);
					if (PID == -1) std::cout << "Process with PID" << enterPID << " is not exist";
				    break;
				case 2:
					std::cout << "Please enter process' name:\n> ";
					std::cin >> enterName;
					PID = selProcessToClose(serMap, enterName);
					if (PID == -1) std::cout << "Process with PID" << enterPID << " is not exist";
					break;
				default:
					std::cout << "Wrong input! Please try again!\n Enter your choice ";
					break;
				case 3:
					std::cout << "--------------------------------------" << '\n';
					std::cout << "PID  " << "\t" << "Name" << '\n';
					for (auto& proc : serMap)
						std::cout << proc.first << '\t'  << proc.second << '\n';
					std::cout << "--------------------------------------" << '\n';
					break;
				case 4:
					return error::E_STOP_SERVER;
				}
			}

			//send header to process close
			if (send(sock, header.closeProc, sizeof(header.closeProc), NULL) == E_RECV_SEND)
				return error::E_SEND_H_CLOSE;

			//send PID process to close
			if (send(sock, (char*)&PID, sizeof(PID), NULL) == E_RECV_SEND)
				return error::E_SEND_PID;

		}
		//receive closing status 
		else if (strcmp(headerBuf, header.closeStatus) == NULL)
		{

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

		//client disconnect
		else if (strcmp(header.disconnect, headerBuf) == NULL)
		{
			// Drop the client

#ifdef _WIN32
			closesocket(sock);
#else defined(__linux__)
			close(sock);
#endif

			FD_CLR(sock, &master);
			std::cout << "> Client " << buffer << (" with SOCKET # ") << sock << " disconnected" << '\n';
		}

	}

}

//TODO: logic if it a system process
//TODO: send to client that it is a system process
int Handler::selProcessToClose(const serializable_map<int, std::string>& map, const std::string& process)
{

#ifdef _WIN32
	std::string sysProc[] = { "svchost.exe", "lsass.exe" , "explorer.exe", "sihost.exe",
		"ntoskrnl.exe", "system",
		"wininit.exe" , " dwm.exe" , "smss.exe", "services.exe",
		"winlogon.exe", "csrss.exe" ,
		"RuntimeBroker.exe", "ApplicationFrameHost.exe",
		"ctfmon.exe", "SecurityHealthService.exe", "spoolsv.exe" };
#endif


	std::vector<int> bufId;
	for (const auto& entry : map)
	{

			if (entry.second == process)
			{
				bufId.emplace_back(entry.first);
			}


	}

	if (!bufId.empty())
	{
		auto it = bufId.begin();
		if (bufId.size() == 1) return *it;
		if (bufId.size() > 1) { std::advance(it, std::rand() % bufId.size()); return *it; };
	}

	return -1; //process is not exist

}

int Handler::selProcessToClose(const serializable_map<int, std::string>& map, const int& PID)
{
	if (map.find(PID) != map.end())
		return PID;
	return -1; // PID in not exist 

}
