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
	  server2Socket = INVALID_SOCKET;
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

	//////////////////////////////////////////////////////////////////////////////////
	//////////// CONNECTING THIS SERVER TO ANOTHER AUTHENTICATE SERVER
	///////////////////////	//////////////////////////////////////////////////////
	struct addrinfo* result2 = NULL, * ptr = NULL, hints2;
	ZeroMemory(&hints2, sizeof(hints2));
	hints2.ai_family = AF_INET;
	hints2.ai_socktype = SOCK_STREAM;
	hints2.ai_protocol = IPPROTO_TCP;
	hints2.ai_flags = AI_PASSIVE;
	intResult = getaddrinfo("localhost", "5001", &hints2, &result2);
	if (intResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", intResult);
		WSACleanup();
		return;
	}
	printf("getaddrinfo success in server2 \n");



	for (ptr = result2; ptr != NULL; ptr = ptr->ai_next)
	{
		server2Socket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (server2Socket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}


		intResult = connect(server2Socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (intResult == SOCKET_ERROR)
		{
			std::cerr << "Can't connect to server!" << std::endl;
			closesocket(server2Socket);
			server2Socket = INVALID_SOCKET;
			WSACleanup();
			return;
		}
		break;
	}
	freeaddrinfo(result2);

	if (server2Socket == INVALID_SOCKET)
	{
		printf("Unable to connect to authenticate server!\n");
		WSACleanup();
		return;
	}

	printf("connected to authenticate server Successfully from server1 \n");

	std::thread receiveThread(&Server::ReceiveMessagesFromAuthenticateServer, this, server2Socket);

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

void Server::ReceiveMessagesFromClient(Client* client)
{
	
	while (true)
	{

		int lengthPrefix;
		int bytesReceived = recv(client->clientSocket, reinterpret_cast<char*>(&lengthPrefix), sizeof(int), 0);
		if (bytesReceived != sizeof(int))
		{
			// Handle error
			break;
		}
		else
		{

			int expectedLength = ntohl(lengthPrefix);
			int error;
			std::vector<uint8_t> receiveDataBuffer(expectedLength);
			bytesReceived = recv(client->clientSocket, reinterpret_cast<char*>(receiveDataBuffer.data()), expectedLength, 0);

			if (bytesReceived == SOCKET_ERROR)
			{
				error = WSAGetLastError();

				if (error == WSAECONNRESET || error == ECONNRESET)
				{
				
					closesocket(client->clientSocket);

					clientList.erase(std::remove(clientList.begin(), clientList.end(), client), clientList.end());
				}
				else
				{
					std::cout << "Server : Receiving message from Client failed with error : " << WSAGetLastError() << std::endl;
				}
				break;
			}
			if (bytesReceived != expectedLength)
			{
				//closesocket(client->clientSocket);
				//clientList.erase(std::remove(clientList.begin(), clientList.end(), client), clientList.end());
				break;
			}
			else
			{
				std::string receivedData(receiveDataBuffer.begin(), receiveDataBuffer.end());
				std::string  commandDesirializer;
				std::string messageCommandSerializer;
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

					CreatewebDeserializer.set_request_id(clientRequestId);
					CreatewebDeserializer.SerializeToString(&commandDesirializer);
					messageAndCommand.set_messagedata(commandDesirializer);

					client->requestId = clientRequestId;
					std::cout << "client->requestId : " << client->requestId << std::endl;
					SendMessageToAuthenticateServer(messageAndCommand, server2Socket);

					clientRequestId++;
					break;
				

				case MessageAndCommand_Command_AUTHENTICATE_WEB:
					AuthenticateWebDeserializer.ParseFromString(messageAndCommand.messagedata());
					std::cout << "EMAIL : " << AuthenticateWebDeserializer.email() << std::endl;
					std::cout << "PASSWORD : " << AuthenticateWebDeserializer.plaintext_password() << std::endl;
					AuthenticateWebDeserializer.set_request_id(clientRequestId);
					AuthenticateWebDeserializer.SerializeToString(&commandDesirializer);
					messageAndCommand.set_messagedata(commandDesirializer);

					client->requestId = clientRequestId;
					std::cout << "client->requestId : " << client->requestId << std::endl;
					SendMessageToAuthenticateServer(messageAndCommand, server2Socket);
					clientRequestId++;
					break;

				}


				
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
	closesocket(client->clientSocket);
}

void Server::ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET clientSocket)
{
	Client* newClient = new Client();
	newClient->clientSocket= clientSocket;
	clientList.push_back(newClient);

    clientSockets.push_back(clientSocket);
    std::thread clientThread(&Server::ReceiveMessagesFromClient, this, newClient);
    clientThreads.push_back(std::move(clientThread));
}

void Server::SendMessagestoClient(MessageAndCommand messagetobeSend, Client* client)
{
	if (client == nullptr)
	{
		std::cout << " Client disconnected .." << std::endl;
		return;
	}
	std::string serializeString;

	messagetobeSend.SerializeToString(&serializeString);

	std::string message = serializeString;
	int messageLength = message.size();
	int lengthToSend = htonl(messageLength);
	std::vector<uint8_t> sendDataBuffer(sizeof(int) + message.size());
	memcpy(sendDataBuffer.data(), &lengthToSend, sizeof(int));
	memcpy(sendDataBuffer.data() + sizeof(int), message.data(), message.size());
	
	send(client->clientSocket, reinterpret_cast<char*>(sendDataBuffer.data()), sendDataBuffer.size(), 0);

}

void Server::SendMessageToAuthenticateServer(MessageAndCommand messagetobeSend, SOCKET server2Scoket)
{
	std::string serializeString;

	messagetobeSend.SerializeToString(&serializeString);

	std::string message = serializeString;
	int messageLength = message.size();
	int lengthToSend = htonl(messageLength);
	std::vector<uint8_t> sendDataBuffer(sizeof(int) + message.size());
	memcpy(sendDataBuffer.data(), &lengthToSend, sizeof(int));
	memcpy(sendDataBuffer.data() + sizeof(int), message.data(), message.size());
	send(server2Scoket, reinterpret_cast<char*>(sendDataBuffer.data()), sendDataBuffer.size(), 0);
}

Client* Server::GetClientWithRequestID(int requestId)
{
	for (Client* client : clientList)
	{
		if (client->requestId == requestId)
			return client;
	}
	return nullptr;
}


void Server::ReceiveMessagesFromAuthenticateServer(SOCKET server2)
{
	int lengthPrefix;
	while (true)
	{

		int bytesReceived = recv(server2, reinterpret_cast<char*>(&lengthPrefix), sizeof(int), 0);
		if (bytesReceived != sizeof(int))
		{
			break;
		}
		else
		{

			int expectedLength = ntohl(lengthPrefix);
			std::vector<uint8_t> receiveDataBuffer(expectedLength);
			bytesReceived = recv(server2, reinterpret_cast<char*>(receiveDataBuffer.data()), expectedLength, 0);

			if (bytesReceived != expectedLength)
			{
				break;
			}
			else
			{
				std::string receivedData(receiveDataBuffer.begin(), receiveDataBuffer.end());

				MessageAndCommand messageAndCommand;
				messageAndCommand.ParseFromString(receivedData);

				CreateAccountWebSuccess CreateAccountWebSuccess;
				CreateAccountWebFailure CreateAccountWebFailure;


				AuthenticateWebSuccess AuthenticateWebSuccess;
				AuthenticateWebFailure AuthenticateWebFailure;
				//////////////THERE WONT BE CREATE AND AUTHENTICATE 
				switch (messageAndCommand.command())
				{
				
				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_SUCCESS:
					CreateAccountWebSuccess.ParseFromString(messageAndCommand.messagedata());
					std::cout << "SUCCESS CREATION - Request ID : " << CreateAccountWebSuccess.request_id() << std::endl;
					std::cout << "SUCCES CREATION - User id : " << CreateAccountWebSuccess.user_id() << std::endl;


					//////// HAVE to SEND TO CLIENTS
					SendMessagestoClient(messageAndCommand, GetClientWithRequestID(CreateAccountWebSuccess.request_id()));

					break;

				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_FAILURE:

					CreateAccountWebFailure.ParseFromString(messageAndCommand.messagedata());
					std::cout << "FAILURE CREATION  - Request ID: " << CreateAccountWebFailure.request_id() << std::endl;
					std::cout << "FAILURE CREATION -  Reason: " << CreateAccountWebFailure.reason() << std::endl;

					SendMessagestoClient(messageAndCommand, GetClientWithRequestID(CreateAccountWebFailure.request_id()));

					break;



				case MessageAndCommand_Command_AUTHENTICATE_WEB_SUCCESS:

					AuthenticateWebSuccess.ParseFromString(messageAndCommand.messagedata());
					std::cout << "SUCCES AUTHENTICATE - Request id: " << AuthenticateWebSuccess.request_id() << std::endl;
					std::cout << "SUCCES AUTHENTICATE - User id : " << AuthenticateWebSuccess.user_id() << std::endl;
					SendMessagestoClient(messageAndCommand, GetClientWithRequestID(AuthenticateWebSuccess.request_id()));
					break;
				case MessageAndCommand_Command_AUTHENTICATE_WEB_FAILURE:

					AuthenticateWebFailure.ParseFromString(messageAndCommand.messagedata());

					std::cout << "FAILURE AUTHENTICATE  - Request ID: " << AuthenticateWebFailure.request_id() << std::endl;
					std::cout << "FAILURE AUTHENTICATE -  Reason: " << AuthenticateWebFailure.reason() << std::endl;

					SendMessagestoClient(messageAndCommand, GetClientWithRequestID(AuthenticateWebFailure.request_id()));

					break;

				}

			}
		}

	}
}




