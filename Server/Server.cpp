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
	 int intResult;
	 serverSocket = INVALID_SOCKET;
	// SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	 //SOCKET server2Socket = socket(AF_INET, SOCK_STREAM, 0);
	 struct addrinfo* result = NULL;
	 struct addrinfo hints;
	 intResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	 if (intResult != 0) 
	 {
		 printf("WSAStartup failed with error: %d\n", intResult);
		 return;
	 }
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
		 return ;
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
	printf("Socket created success in server \n");


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
	printf(" Listen success in server \n");


	 //sockaddr_in server2Hint;
	 //server2Hint.sin_family = AF_INET;
	 //server2Hint.sin_port = htons(54001); // Assuming Server2 is on a different port
	 //inet_pton(AF_INET, "127.0.0.1", &server2Hint.sin_addr);
	AcceptClientConnections(serverSocket);



	
	 
	ServerClose();

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

void Server::AcceptClientConnections(SOCKET serverSocket)
{
	while (true)
	{
		sockaddr_in client;
		int clientSize = sizeof(client);

		SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
		std::cout << "NEW CLIENT JOINED >> " << std::endl;
		if (clientSocket == INVALID_SOCKET)
		{
			std::cerr << "Invalid client socket." << std::endl;
			continue;
		}
		// ReceiveAndPrintIncomingMessage(clientSocket);
		ReceiveAndPrintIncomingMessageOnSeparateThread(clientSocket);
	}
}

void Server::ReceiveAndPrintIncomingMessage(SOCKET clientSocket)
{
	
	while (true)
	{

		int lengthPrefix;
		int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&lengthPrefix), sizeof(int), 0);
		if (bytesReceived != sizeof(int))
		{
			// Handle error
		}
		else
		{

			int expectedLength = ntohl(lengthPrefix);
			std::vector<uint8_t> receiveDataBuffer(expectedLength);
			bytesReceived = recv(clientSocket, reinterpret_cast<char*>(receiveDataBuffer.data()), expectedLength, 0);

			if (bytesReceived != expectedLength)
			{

			}
			else
			{
				std::string receivedData(receiveDataBuffer.begin(), receiveDataBuffer.end());

				MessageAndCommand messageAndCommand;
				messageAndCommand.ParseFromString(receivedData);
				CreateAccountWeb CreatewebDeserializer;
				AuthenticateWeb AuthenticateWebDeserializer;
				switch (messageAndCommand.command())
				{

				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB:
					CreatewebDeserializer.ParseFromString(messageAndCommand.messagedata());
					std::cout << "EMAIL : " << CreatewebDeserializer.email() << std::endl;
					std::cout << "PASSWORD : " << CreatewebDeserializer.plaintext_password() << std::endl;
					break;
				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_SUCCESS:
					break;

				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_FAILURE:
					break;



				case MessageAndCommand_Command_AUTHENTICATE_WEB:
					AuthenticateWebDeserializer.ParseFromString(messageAndCommand.messagedata());
					std::cout << "EMAIL : " << AuthenticateWebDeserializer.email() << std::endl;
					std::cout << "PASSWORD : " << AuthenticateWebDeserializer.plaintext_password() << std::endl;
					break;
				case MessageAndCommand_Command_AUTHENTICATE_WEB_FAILURE:
					break;
				case MessageAndCommand_Command_AUTHENTICATE_WEB_SUCCESS:
					break;

				}


				SendMessagestoClient(clientSocket);
			}
		}

		char buf[512];


		///////////////////////////////// // Connect to Server2
		///////
		////////////////////////
		 //// Send the message to Server2
		 //send(server2Socket, buf, strlen(buf) + 1, 0);

		 //// Receive a completion message from Server2
		 //ZeroMemory(buf, sizeof(buf));
		 //recv(server2Socket, buf, sizeof(buf), 0);
		 //std::cout << "Received from Server2: " << buf << std::endl;
		 ////////////////////////////////////////////////////////////////////////////////////////////
	}
	closesocket(clientSocket);
}

void Server::ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET clientSocket)
{
    clientSockets.push_back(clientSocket);

    std::thread clientThread(&Server::ReceiveAndPrintIncomingMessage, this, clientSocket);
    clientThreads.push_back(std::move(clientThread));
}

void Server::SendMessagestoClient(SOCKET clientSocket)
{
	MessageAndCommand messageAndCommand;
	std::string serializeString;


	std::string  CreateAccountWebSuccessserilizer;
	CreateAccountWebSuccess success;
	success.set_user_id(1);
	success.set_request_id(2);
	success.SerializeToString(&CreateAccountWebSuccessserilizer);

	messageAndCommand.set_command(MessageAndCommand_Command_CREATE_ACCOUNT_WEB_SUCCESS);
	messageAndCommand.set_messagedata(CreateAccountWebSuccessserilizer);
	messageAndCommand.SerializeToString(&serializeString);





	std::string message = serializeString;
	int messageLength = message.size();
	int lengthToSend = htonl(messageLength);
	std::vector<uint8_t> sendDataBuffer(sizeof(int) + message.size());
	memcpy(sendDataBuffer.data(), &lengthToSend, sizeof(int));
	memcpy(sendDataBuffer.data() + sizeof(int), message.data(), message.size());

	 send(clientSocket, reinterpret_cast<char*>(sendDataBuffer.data()), sendDataBuffer.size(), 0);
}




