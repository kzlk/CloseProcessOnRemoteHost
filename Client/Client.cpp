#include "Client.h"
#include "CSerializeMap.h"
#include "Process.h"

#define HEADER 20
using namespace std;

// TODO: (22.08.2022) 
// push commit to github
// creaet


SOCKET sock;
Process proc;

//TODO: enum for status of closing process
enum class header
{

};

//handling all receiveing message in new thread
void clientHandler()
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

	int msg_size;
	while (true)
	{
		char close_header[HEADER] = "close_proc";
		//recv(sock, (char*)&msg_size, sizeof(int), NULL);
		//char* msg = new char[msg_size + 1];
		char msg[20];
		recv(sock, msg, 20, NULL);
		//for(int i = 0; i< msg_size;++i)
		std::cout << msg << std::endl;
		//delete[] msg;

		if (strcmp(close_header, msg) == 0)
		{
			int PID{};
			if (recv(sock, (char*)&PID, sizeof(PID), NULL) == -1)
				std::cerr << "Cannot receive PID from client" << '\n';
			if (proc.closeProcessByPID(PID) != 0)
				std::cout << "Process with PID " << PID << " closed successful!";
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

	sockaddr_in hint;
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
	std::cout << "Connected!\n";

	//do-wile loop to send and receive data



	char buf[4096];
	std::string userInput;
	Process process; // Class Process for process manupulation

	//std::thread thread(clientHandler); // thread to start clientHandler in new thread 

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, NULL, NULL, NULL);
	int j = 1;
	std::string msg1;
	do
	{
		//Promt the user for some text 

		//std::cout << "Sending list of process to server...\n";
		//	getline(std::cin, userInput);
			//std::cin >> userInput;
			//std::map<unsigned, std::string> map = process.processNamePID();

#ifdef _WIN32
		auto serMap = process.processNamePID();
		if (serMap.empty()) std::cerr << "Could not get Windows processes " << '\n';
#endif //
		//TODO: friendly user console interface

		std::vector<char> byteStream = serMap.serialize();
		serializable_map<int, std::string> serMap2;
		serMap2.deserialize(byteStream);

		int len;
		len = byteStream.size();

		//if (userInput.size() > 0) //make sure that user has typed in something 
		//{
			//Send the text

			//int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
		//sendall(sock, byteStream, &len);
		int sendSize = send(sock, (char*)&len, sizeof(int), NULL); //SEND TO SERVER SIZE OF MAP
		if (sendSize == -1)
		{
			perror("Cannot send size of map");

		}
		//int sendSize2 = send(sock, (char*)&len, sizeof(int), NULL);
		//sendall(sock, byteStream, &len);
		//int sendMap = sendall(sock, byteStream, &len);


		if (sendall(sock, byteStream, &len) == -1)
		{
			perror("Cannot send serialized map");
			std::cout << len << " bytes sent" << '\n';
			system("pause");
		}

		//recv()

		//int result = sendall(sock, byteStream, &len);

		//int sendResult = send(sock, map, userInput.size() + 1, 0);
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


	//std::cin >> msg1;
	//int msg_size = msg1.size();
	//send(sock,(char*)&msg_size, sizeof(int), NULL);
	//send(sock, msg1.c_str(), msg1.size(), NULL);
		Sleep(10);
		std::cin.get();
		std::cin.get();

		

		system("pause"); // wait for user response

	} while (true);
	//thread.join();


	//gracefully close down everything
	closesocket(sock);
	WSACleanup();


	return 0;
}


