#include "CServerHandler.h"



E_CODE_MESSAGE Handler::clientHandler(const SOCKET& sock, const char* buffer, fd_set& master)
{
	static int getPID; //input from user PID
	static std::string getName; //input from user names
	static bool isPIDSent;
	static int PID;
	//ZeroMemory(headerBuf, MESSAGE_HEADER_SIZE); 
	memset(headerBuf, 0, MESSAGE_HEADER_SIZE);

	while (true) 
	{
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
		else if (strcmp(header.getPID, headerBuf) == NULL)
		{
			//recieve PID
			if (recv(sock, (char*)&getPID, sizeof(int), NULL) == E_RECV_SEND)
			{
				isPIDSent = true;
				return error::E_GET_PID;
			}

		}
		else if (strcmp(header.getName, headerBuf) == NULL)
		{
			ZeroMemory(buf, 4096);
			//received name
			const int bytesReceived = recv(sock, (char*)&buf, 4096, NULL);
			if (bytesReceived != E_RECV_SEND)
			{
				getName = std::string(buf, 0, bytesReceived);
				isPIDSent = false;

			}
			else
			{
				return error::E_GET_NAME;
			}

		}
		else if (strcmp(header.selProc, headerBuf) == NULL)
		{
			//sel process to close

			if (isPIDSent)
			{
				//return sel proc by PID
				PID = selProcessToClose(serMap, getPID);
			}
			else
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
int Handler::selProcessToClose(const serializable_map<int, std::string>& map, const std::string& process)
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
#endif
	//TODO: linux lin_sys_proc.txt

	//TODO: logic for chosing the process to close

	std::vector<int> bufId;
	for (const auto& entry : map)
	{

		for (auto& it : sysProc)
		{
#ifdef _WIN32
			if (entry.second == it)
			{
				//TODO: logic if it a system process
				continue;
			}
#endif

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

}
int Handler::selProcessToClose(const serializable_map<int, std::string>& map, const int& PID)
{
	if (map.find(PID) != map.end())
		return PID;
	return -1; // PID in not exist 

}
