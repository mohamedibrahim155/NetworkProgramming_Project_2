#include "AuthenticateServer.h"



AuthenticateServer::AuthenticateServer()
{
}

AuthenticateServer::~AuthenticateServer()
{
}

void AuthenticateServer::Initialize()
{
	WSADATA wsData;
	int intResult;
	serverSocket = INVALID_SOCKET;
	// SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKET server2Socket = INVALID_SOCKET;
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


	AcceptClientConnections(serverSocket);


	ServerClose();



}


void AuthenticateServer::ServerClose()
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
void AuthenticateServer::AcceptClientConnections(SOCKET serverSocket)
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

void AuthenticateServer::ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET server1Socket)
{
	clientSockets.push_back(server1Socket);

	std::thread clientThread(&AuthenticateServer::ReceiveAndPrintIncomingMessage, this, server1Socket);
	clientThreads.push_back(std::move(clientThread));
}


void AuthenticateServer::ReceiveAndPrintIncomingMessage(SOCKET server1Socket)
{

	while (true)
	{

		int lengthPrefix;
		int bytesReceived = recv(server1Socket, reinterpret_cast<char*>(&lengthPrefix), sizeof(int), 0);
		if (bytesReceived != sizeof(int))
		{
			// Handle error
		}
		else
		{

			int expectedLength = ntohl(lengthPrefix);
			std::vector<uint8_t> receiveDataBuffer(expectedLength);
			bytesReceived = recv(server1Socket, reinterpret_cast<char*>(receiveDataBuffer.data()), expectedLength, 0);

			if (bytesReceived != expectedLength)
			{

			}
			else
			{
				std::string receivedData(receiveDataBuffer.begin(), receiveDataBuffer.end());

				std::string dataSerializer;
				MessageAndCommand messageAndCommand;
				messageAndCommand.ParseFromString(receivedData);
				CreateAccountWeb CreatewebDeserializer;
				AuthenticateWeb AuthenticateWebDeserializer;


				CreateAccountWebSuccess CreateAccountSuccess;
				CreateAccountWebFailure CreateAccountFailure;

				AuthenticateWebSuccess AuthenticateSuccess;
				AuthenticateWebFailure AuthenticateFailure;


				int userId;
				std::string salt;
				std::string hashPassword;
				std::string creationDate;
				switch (messageAndCommand.command())
				{

#pragma region REGISTER



				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB:
					CreatewebDeserializer.ParseFromString(messageAndCommand.messagedata());
					std::cout << "EMAIL : " << CreatewebDeserializer.email() << std::endl;
					std::cout << "PASSWORD : " << CreatewebDeserializer.plaintext_password() << std::endl;
					std::cout << "Request id : " << CreatewebDeserializer.request_id() << std::endl;

					int result;

					 salt = generateSalt();

					 hashPassword = hash(salt + CreatewebDeserializer.plaintext_password());

					result = mysql->AddAccount(CreatewebDeserializer.email().c_str(), salt.c_str(), hashPassword.c_str());
					if (result == 1)
					{
						result = mysql->AddNewUser(userId);
						result = mysql->UpdateUserID(CreatewebDeserializer.email().c_str(), userId);

						CreateAccountSuccess.set_request_id(CreatewebDeserializer.request_id());
						CreateAccountSuccess.set_user_id(userId);

						CreateAccountSuccess.SerializeToString(&dataSerializer);

						messageAndCommand.set_command(MessageAndCommand_Command_CREATE_ACCOUNT_WEB_SUCCESS);
						messageAndCommand.set_messagedata(dataSerializer);

						SendMessagestoServer1(messageAndCommand, server1Socket);
					}
					else
					{
						CreateAccountFailure.set_request_id(CreatewebDeserializer.request_id());
						if (result == 0)
						{
							CreateAccountFailure.set_reason(CreateAccountWebFailure_Reason_ACCOUNT_ALREADY_EXISTS);
						}
						else if(result == -1)
						{

							CreateAccountFailure.set_reason(CreateAccountWebFailure_Reason_INTERNAL_SERVER_ERROR);
						}

						CreateAccountFailure.SerializeToString(&dataSerializer);

						messageAndCommand.set_command(MessageAndCommand_Command_CREATE_ACCOUNT_WEB_FAILURE);
						messageAndCommand.set_messagedata(dataSerializer);

						SendMessagestoServer1(messageAndCommand, server1Socket);
					}
					

					break;
#pragma endregion	
				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_SUCCESS:
					break;

				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_FAILURE:
					break;



				case MessageAndCommand_Command_AUTHENTICATE_WEB:
					AuthenticateWebDeserializer.ParseFromString(messageAndCommand.messagedata());
					std::cout << "EMAIL : " << AuthenticateWebDeserializer.email() << std::endl;
					std::cout << "PASSWORD : " << AuthenticateWebDeserializer.plaintext_password() << std::endl;
					std::cout << "Request id : " << AuthenticateWebDeserializer.request_id() << std::endl;
					userId = 0;
					creationDate = "";
					result = mysql->AuthenticateAccount(AuthenticateWebDeserializer.email().c_str(), AuthenticateWebDeserializer.plaintext_password().c_str(), userId);
					if (result ==1)
					{
						mysql->UpdateLastLogin(userId, creationDate);

						AuthenticateSuccess.set_request_id(AuthenticateWebDeserializer.request_id());
						AuthenticateSuccess.set_user_id(userId);
						AuthenticateSuccess.set_creation_date(creationDate);

						AuthenticateSuccess.SerializeToString(&dataSerializer);

						messageAndCommand.set_command(MessageAndCommand_Command_AUTHENTICATE_WEB_SUCCESS);
						messageAndCommand.set_messagedata(dataSerializer);

						SendMessagestoServer1(messageAndCommand, server1Socket);

					}
					else
					{
						AuthenticateFailure.set_request_id(AuthenticateWebDeserializer.request_id());

						if (result == 2 || result == 0)
						{
							AuthenticateFailure.set_reason(AuthenticateWebFailure_Reason_INVALID_CREDENTIALS);
						}
						else if (result == -1)
						{
							AuthenticateFailure.set_reason(AuthenticateWebFailure_Reason_INTERNAL_SERVER_ERROR);
						}

						AuthenticateFailure.SerializeToString(&dataSerializer);

						messageAndCommand.set_command(MessageAndCommand_Command_AUTHENTICATE_WEB_FAILURE);
						messageAndCommand.set_messagedata(dataSerializer);

						SendMessagestoServer1(messageAndCommand, server1Socket);

					}


					break;
				case MessageAndCommand_Command_AUTHENTICATE_WEB_FAILURE:
					break;
				case MessageAndCommand_Command_AUTHENTICATE_WEB_SUCCESS:
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
	closesocket(server1Socket);
}

void AuthenticateServer::SendMessagestoServer1(MessageAndCommand messagetoSend, SOCKET server1Socket)
{
	
	std::string serializeString;
	messagetoSend.SerializeToString(&serializeString);





	std::string message = serializeString;
	int messageLength = message.size();
	int lengthToSend = htonl(messageLength);
	std::vector<uint8_t> sendDataBuffer(sizeof(int) + message.size());
	memcpy(sendDataBuffer.data(), &lengthToSend, sizeof(int));
	memcpy(sendDataBuffer.data() + sizeof(int), message.data(), message.size());

	send(server1Socket, reinterpret_cast<char*>(sendDataBuffer.data()), sendDataBuffer.size(), 0);
}

void AuthenticateServer::SetSQLUtil(MySQLUtil* mysql)
{
	this->mysql = mysql;
	if (mysql!=nullptr)
	{
		std::cout << "SQL not null" << std::endl;
		
	}
}

std::string AuthenticateServer::generateSalt(int length)
{
	const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	const int charsetSize = sizeof(charset) - 1;  // Exclude null terminator

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(0, charsetSize - 1);

	std::string salt;
	salt.reserve(length);

	for (int i = 0; i < length; ++i) {
		salt.push_back(charset[dist(rng)]);
	}

	return salt;
}


