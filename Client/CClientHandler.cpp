#include "CClientHandler.h"

int CClientHandler::iContinue(const SOCKET& sock) const
{
	return send(sock, header.continueWork, MESSAGE_HEADER_SIZE, NULL);
}

int CClientHandler::iDisconect(const SOCKET& sock) const
{
	return send(sock, header.disconnect, MESSAGE_HEADER_SIZE, NULL);
}

//handle receiving server header
E_CODE_MESSAGE CClientHandler::clientHandler(const SOCKET& sock)
{
	int PID{};
	int isSuccess = 0;
	int isFailed = -1;
	char headerBuf[MESSAGE_HEADER_SIZE]{};
    memset(headerBuf, 0, MESSAGE_HEADER_SIZE);

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
			if (proc->closeProcessByPID(PID) != ERROR_CLOSE)
			{
				//TODO: name also output
				//send status is success
				if (send(sock, (char*)&isSuccess, sizeof(isSuccess), NULL) == E_RECV_SEND)
					return msg::E_SEND_STATUS;
				std::cout << "> Process "<< serMap.find(PID)->second <<" with PID " << PID << " closed successful" << '\n';
				return msg::SUCCESS;

			}
			else
			{
				//send status is failure
				if (send(sock, (char*)&isFailed, sizeof(isFailed), NULL) == E_RECV_SEND)
					return msg::E_SEND_STATUS;
				std::cout << "> Process " << serMap.find(PID)->second << "with PID " << PID << " close failed" << '\n';
				return msg::FAILURE;
			}



		}

		//send to server running list of process
		else if (strcmp(header.getListProc, headerBuf) == NULL)
		{

			serMap = proc->processNamePID();
			if (serMap.empty()) { return msg::E_GET_WIN_PROC; }

			std::vector<char> byteStream = serMap.serialize();

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
		}

		//receive header to shutdown client app because server is stopping
		else if(strcmp(header.closeServer, headerBuf) == NULL)
		{
		char buf[1000];

		const int bytesReceived = recv(sock, buf, sizeof(buf), 0);
		if (bytesReceived == E_RECV_SEND) return msg::E_RECV_EXIT;
			std::cout << std::string(buf, 0, bytesReceived);
			return msg::SHUTDOWN_CLIENT;
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
