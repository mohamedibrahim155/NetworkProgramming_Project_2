#include "Server.h"


Server::Server()
{
}

Server::~Server()
{
}

 void Server::Initialize()
{

	 // Initialize Winsock
	 WSADATA wsData;
	 WORD ver = MAKEWORD(2, 2);
	 WSAStartup(ver, &wsData);

	 // Create socket
	 SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	 SOCKET server2Socket = socket(AF_INET, SOCK_STREAM, 0);
	 // Bind the socket to an IP address and port
	 sockaddr_in hint;
	 hint.sin_family = AF_INET;
	 hint.sin_port = htons(54000);
	 hint.sin_addr.S_un.S_addr = INADDR_ANY;

	 sockaddr_in server2Hint;
	 server2Hint.sin_family = AF_INET;
	 server2Hint.sin_port = htons(54001); // Assuming Server2 is on a different port
	 inet_pton(AF_INET, "127.0.0.1", &server2Hint.sin_addr);

	 bind(serverSocket, (sockaddr*)&hint, sizeof(hint));

	 // Tell Winsock the socket is for listening
	 listen(serverSocket, SOMAXCONN);


	

	 connect(server2Socket, (sockaddr*)&server2Hint, sizeof(server2Hint));



	 // Wait for a connection
	 sockaddr_in client;
	 int clientSize = sizeof(client);

	 SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);

	 // Receive message from client
	 char buf[4096];
	 ZeroMemory(buf, sizeof(buf));
	 recv(clientSocket, buf, sizeof(buf), 0);
	 std::cout << "Received from client: " << buf << std::endl;

	 // Connect to Server2
	
	
	 // Send the message to Server2
	 send(server2Socket, buf, strlen(buf) + 1, 0);

	 // Receive a completion message from Server2
	 ZeroMemory(buf, sizeof(buf));
	 recv(server2Socket, buf, sizeof(buf), 0);
	 std::cout << "Received from Server2: " << buf << std::endl;

	 // Send the completion message back to the client
	 send(clientSocket, buf, strlen(buf) + 1, 0);

	 // Close connection to Server2
	 closesocket(server2Socket);

	 // Close connection to the client
	 closesocket(clientSocket);

	 // Cleanup Winsock
	 WSACleanup();

	 return;

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
	char buf[4096];
	ZeroMemory(buf, sizeof(buf));
	while (true)
	{


	
		int amountReceived = recv(clientSocket, buf, sizeof(buf), 0);
		if (amountReceived > 0)
		{
			std::cout << "Received from Client: " << buf << std::endl;

			/////////////////////////////////////////////////
			// 
			// SEND MESSAGES TO  SERVER 2 AND  RECEIVE IT 
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




