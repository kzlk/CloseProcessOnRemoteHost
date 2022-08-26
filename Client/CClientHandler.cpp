#include "CClientHandler.h"

int CClientHandler::iDisconect(const SOCKET& sock)
{
	return send(sock, header.disconnect, MESSAGE_HEADER_SIZE, NULL);
}

//handle receiving message
//TODO: exit from cycle
//TODO: output name of process by received PID
E_CODE_MESSAGE CClientHandler::clientHandler(const SOCKET& sock)
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
			return msg::E_H_CLOSE_PROC;

		//received header to close process
		if (strcmp(header.closeProc, headerBuf) == NULL)
		{
			//receive PID 
			if (recv(sock, (char*)&PID, sizeof(PID), NULL) == E_RECV_SEND)
				return msg::E_RECV_PID;
			std::cout << "Server > : received PID " << PID << '\n';

			//send closestatus header 
			if (send(sock, header.closeStatus, sizeof(header.closeStatus), NULL) == E_RECV_SEND)
				return msg::E_SEND_H_STATUS;

			//close process
			if (proc->closeProcessByPID(PID) != NULL)
			{
				//TODO: name also output
				//send status is success
				if (send(sock, (char*)&isSuccess, sizeof(isSuccess), NULL) == E_RECV_SEND)
					return msg::E_SEND_STATUS;
				std::cout << "> Process ...... with PID " << PID << " closed successful" << '\n';
				return msg::SUCCESS;

			}
			else
			{
				//send status is failure
				if (send(sock, (char*)&isFailed, sizeof(isFailed), NULL) == E_RECV_SEND)
					return msg::E_SEND_STATUS;
				std::cout << "> Process ...... with PID " << PID << " close failed" << '\n';
				return msg::FAILURE;
			}



		}
		else if (strcmp(header.getListProc, headerBuf) == NULL)
		{
#ifdef _WIN32
			auto serMap = proc->processNamePID();
			if (serMap.empty()) { return msg::E_GET_WIN_PROC; }
#endif //
			//TODO: friendly user console interface

			std::vector<char> byteStream = serMap.serialize();
			//serializable_map<int, std::string> serMap2;
			//serMap2.deserialize(byteStream);
			int len = byteStream.size();

			//send header list of processes
			int sendHeaderFor = send(sock, header.listProc, MESSAGE_HEADER_SIZE, NULL);
			if (sendHeaderFor == E_RECV_SEND) { return msg::E_SEND_H_LIST; }

			//send to server size of map(processes list)
			int sendSize = send(sock, (char*)&len, sizeof(int), NULL); //
			if (sendSize == -1) { return msg::E_SIZE_MAP; }

			//send serialized map to server
			if (sendall(sock, byteStream, &len) == E_RECV_SEND)
			{
				std::cout << "Serialized map --> " << len << " bytes sent" << '\n';
				return msg::E_SEND_MAP;
			}

			//enter PID or name of process
			int choice{};
			int enterPID{};
			std::string enterName{};
			std::cout << "How do you want to close the process?" << '\n';
			std::cout << "1) By PID : enter -> 1 \n2) By name : enter -> 2\n3) Exit: enter -> 3\n";

			while (choice != 2 && choice != 1)
			{
				std::cout << '>';
				std::cin >> choice;
				switch (choice)
				{
				case 1:
					std::cout << "Please enter process' PID: ";
					std::cout << '>';
					std::cin >> enterPID;

					//send header that we enter PID
					if (send(sock, header.getPID, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
						return msg::E_SEND_H_PID;

					//send input PID
					if (send(sock, (char*)&enterPID, sizeof(int), NULL) == E_RECV_SEND)
						return msg::E_SEND_PID;
					break;

				case 2:
					std::cout << "Please enter process' name : ";
					std::cout << '>';
					std::cin >> enterName;

					//send header that we enter NAME
					if (send(sock, header.getName, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
						return msg::E_SEND_H_NAME;

					//send input NAME
					if (send(sock, enterName.c_str(), enterName.size() + 1, NULL) == E_RECV_SEND)
						return msg::E_SEND_PID;
					break;
				default:
					std::cout << "Wrong input! Please try again!\n Enter your choice ";
					break;

				case 3:
					if (iDisconect(sock) == E_RECV_SEND) return msg::E_SEND_H_DISC;
				}
			}

			//send header to select process
			if (send(sock, header.selProc, MESSAGE_HEADER_SIZE, NULL) == E_RECV_SEND)
				return msg::E_SEND_H_PID;

		}

	}

}

int CClientHandler::sendall( int s, const std::vector<char>& buf, int* len)
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
	return n == -1 ? -1 : 0; //  -1 when msg, 0 when success
}
