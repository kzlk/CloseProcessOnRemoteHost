#include "Server.h"


//fucn for receive all data over socket
int recvall(int s, vector<char>& destination, int* len)
{
	int total = 0;
	int bytesleft = *len;
	int n;
	while (total < *len) {
		n = recv(s, (destination.data()) + total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total;
	return n == -1 ? -1 : 0;
}

//TODO: make logic which process will be close
//func to select process will be close
auto selProcessToClose(serializable_map<int, std::string>& map)
{

	//TODO: read from file win_sys_proc.txt
#ifdef _WIN32
	std::string sysProc[] = { "svchost.exe", "lsass.exe" , "explorer.exe", "sihost.exe",
		"ntoskrnl.exe", "system",
		"wininit.exe" , " dwm.exe" , "smss.exe", "services.exe",
		"winlogon.exe", "csrss.exe" ,
		"RuntimeBroker.exe", "ApplicationFrameHost.exe",
		"ctfmon.exe", "SecurityHealthService.exe", "spoolsv.exe" };
	const std::string process = "chrome.exe"; //hardcode for testing
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

	return 0;
};

//TODO: func which start in new thread and recv header from client and do some job
void clientHandler() {}

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
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Create the master file descriptor set and zero it
	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

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
				SOCKET client = accept(listening, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				string welcomeMsg = "Message from server!\r\n";

				//send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				int len{};

				// Receive num of bytes in map<> from client
				int bytesIn = recv(sock, (char*)&len, sizeof(len), 0);

				//buf to store stream of bytes
				std::vector<char> recvDataByte;
				recvDataByte.resize(len);

				//recieve serialized map data
				int bytesIn2 = recv(sock, ((char*)recvDataByte.data()), len, 0);

				//deserialize received map
				serializable_map<int, std::string> serMap;
				serMap.deserialize(recvDataByte);

				//get PID 
				auto PID = selProcessToClose(serMap);

				char headerBuffer[20] = "close_proc";
				//char close_header[20] 
				if (send(sock, headerBuffer, sizeof(headerBuffer), NULL));

				if (send(sock, (char*)&PID, sizeof(PID), NULL) == -1)
					std::cerr << "SEND PID TO CLIENT " << sock << " FAILED" << '\n';


				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{

					if (buf[0] == '\\')
					{
						string cmd = string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							running = false;
							break;
						}

						// Unknown command
						continue;
					}
					//To communicate with other client
					//*********************
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock == sock)
						{
							ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r\n";
							string strOut = ss.str();
							//if(sock == master.fd_array[i])
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
					//**************************

				}
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