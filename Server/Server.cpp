#include "Server.h"


Server::Server()
{
}

Server::~Server()
{
}

 void Server::Initialize()
{

	WSADATA wsaData;
	int intResult;
	serverSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;



	int iSendResult;
	intResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (intResult != 0) {
		printf("WSAStartup failed with error: %d\n", intResult);
		return;
	}
	printf("SAStartup  Successful in client\n");


	printf("WSAStartup success in server \n");
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	intResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (intResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", intResult);
		WSACleanup();
		return;
	}

	printf("getaddrinfo success in server \n");


	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}


	intResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
	if (intResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(serverSocket);
		WSACleanup();
		return;
	}
	printf("Socket  binded  success in server \n");
	freeaddrinfo(result);


	intResult = listen(serverSocket, SOMAXCONN);
	if (intResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return;
	}



	//////////ACCEPT CLIENT"S SCOKET HERE
	// 
	///////////////////
	///////////////////
	AcceptClientConnections();

	ServerClose();
	// cleanup
	//closesocket(ClientSocket);

}

void Server::ServerClose()
{
	int intResult = shutdown(serverSocket, SD_SEND);
	if (intResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return;
	}
	printf(" shutdown success in server \n");


	for (auto& thread : clientThreads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
	WSACleanup();
	clientSockets.clear();
}

void Server::AcceptClientConnections()
{
	std::cout << "Ready to accept connections..." << std::endl;
	while (true)
	{
		sockaddr_in clientAddr;
		int clientSize = sizeof(clientAddr);

	SOCKET  clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

		if (clientSocket == INVALID_SOCKET)
		{
			std::cerr << "Invalid client socket." << std::endl;
			continue;
		}

		////RECEIVE MESSAAGES FROM CLIENT
		ReceiveAndPrintIncomingMessageOnSeparateThread(clientSocket);
	}
}

void Server::ReceiveAndPrintIncomingMessage(SOCKET clientSocket)
{
	const int buffersize = DEFAULT_BUFLEN;
	while (true)
	{


		std::vector<uint8_t> recvBuffer(buffersize);
		int amountReceived = recv(clientSocket, reinterpret_cast<char*>(&recvBuffer[0]), buffersize, 0);
		if (amountReceived > 0)
		{


			/////////////////////////////////////////////////
			// 
			// SEND MESSAGES TO ANOTHER SERVER AND  RECEIVE IT 
			// 
			// 
			// ///////////////////////////////////////////////



		}

		if (amountReceived == 0)
		{
			//Client Disconnected ...
		//	RemoveSocketInRoom(clientSocket);
			std::cout << "Client Disconnected .." << std::endl;

			break;
		}
		else if (amountReceived == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				// Client forcefully disconnected
			///	RemoveSocketInRoom(clientSocket);
				break;
			}
		}


	}
	closesocket(clientSocket);
}

void Server::ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET clientSocket)
{
    clientSockets.push_back(clientSocket);

    std::thread clientThread(&Server::ReceiveAndPrintIncomingMessage, this, clientSocket);
    clientThreads.push_back(std::move(clientThread));
}




