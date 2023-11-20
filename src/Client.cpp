#include "Client.h"

Client::Client()
{
}

Client::~Client()
{
}

void Client::Initialize()
{
    WSADATA wsData;
	int iResult;
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	clientSocket = INVALID_SOCKET;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}
	printf("SAStartup  Successful in client\n");

  
    // Connect to Server1
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);

	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}
	printf("getaddrinfo  Successful in client\n");

	
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		clientSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return ;
		}


		iResult = connect(clientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			std::cerr << "Can't connect to server!" << std::endl;
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			WSACleanup();
			return ;
		}
		break;
	}
	
	freeaddrinfo(result);
    
	if (clientSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}
	printf("connected to server Successfully in client\n");

	std::thread receiveThread(&Client::ReceiveMessages, this, clientSocket);

	SendToServer(clientSocket);



	

    // Receive a message from Server1
   /* char buf[4096];
    ZeroMemory(buf, sizeof(buf));
    recv(clientSocket, buf, sizeof(buf), 0);
    std::cout << "Received from Server1: " << buf << std::endl;*/
    system("Pause");
    // Close connection to Server1
    closesocket(clientSocket);

    // Cleanup Winsock
    WSACleanup();

}

void Client::CloseClient()
{
	int iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return ;
	}
	printf("shutdown Successfully in client\n");


	// cleanup
	closesocket(clientSocket);
	WSACleanup();
}

void Client::ReceiveMessages(SOCKET sock)
{
	int lengthPrefix;
	while (true)
	{
		
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


				CreateAccountWebSuccess CreateAccountWebSuccess;
				CreateAccountWebFailure CreateAccountWebFailure;


				AuthenticateWebSuccess AuthenticateWebSuccess;
				AuthenticateWebFailure AuthenticateWebFailure;

				switch (messageAndCommand.command())
				{

				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB:
					CreatewebDeserializer.ParseFromString(messageAndCommand.messagedata());
					std::cout << "EMAIL : " << CreatewebDeserializer.email() << std::endl;
					std::cout << "PASSWORD : " << CreatewebDeserializer.plaintext_password() << std::endl;
					break;
				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_SUCCESS:

					std::cout << "ACCOUNT REGISTERED SUCCESSFULLY" << std::endl;
					break;

				case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_FAILURE:
					CreateAccountWebFailure.ParseFromString(messageAndCommand.messagedata());

					std::cout << " FAILED TO REGISTER :";
					switch (CreateAccountWebFailure.reason())
					{
					case CreateAccountWebFailure_Reason_ACCOUNT_ALREADY_EXISTS:
						std::cout << " ACCOUNT ALREADY EXISTS";
						break;
					case CreateAccountWebFailure_Reason_INVALID_PASSWORD:
						std::cout << " INVALID PASSWORD";
						break;
					case CreateAccountWebFailure_Reason_INTERNAL_SERVER_ERROR:
						std::cout << " INTERNAL SERVER ERROR";
						break;
					}
					std::cout << "CREATED ACCOUNT SUCCESFULLY" << std::endl;
					break;



				case MessageAndCommand_Command_AUTHENTICATE_WEB:
					AuthenticateWebDeserializer.ParseFromString(messageAndCommand.messagedata());
					std::cout << "EMAIL : " << AuthenticateWebDeserializer.email() << std::endl;
					std::cout << "PASSWORD : " << AuthenticateWebDeserializer.plaintext_password() << std::endl;
					break;
				case MessageAndCommand_Command_AUTHENTICATE_WEB_FAILURE:
				

					AuthenticateWebFailure.ParseFromString(messageAndCommand.messagedata());
					std::cout << " ACCOUNT AUTHENTICATED FAILED :";

					switch (AuthenticateWebFailure.reason())
					{
					case AuthenticateWebFailure_Reason_INTERNAL_SERVER_ERROR:
						std::cout << "INTERNAL SERVER ERROR";
						break;

					case AuthenticateWebFailure_Reason_INVALID_CREDENTIALS:
						std::cout << "INVALID CREDENTIALS";
						break;
					}

					break;
				case MessageAndCommand_Command_AUTHENTICATE_WEB_SUCCESS:
					std::cout << " ACCOUNT AUTHENTICATE SUCCESS" << std::endl;
					break;

				}

			}
		}

	}
}

void Client::SendToServer(SOCKET sock)
{
	while (true)
	{

		std::string serializeString;
		std::string userInput, email, password;
		std::cout << "ENTER COMMAND REGISTER OR AUTHENTICATE : ";
		std::cin >> userInput;
		std::cout << "ENTER EMAIL : ";
		std::cin >> email;
		std::cout << "ENTER PASSWORD : ";
		std::cin >> password;

		MessageAndCommand messageAndCommand;
		if (userInput == "REGISTER")
		{
			std::string createwebSerializer;
			CreateAccountWeb createWeb;
			createWeb.set_email(email);
			createWeb.set_plaintext_password(password);
			createWeb.SerializeToString(&createwebSerializer);

			messageAndCommand.set_command(MessageAndCommand_Command_CREATE_ACCOUNT_WEB);
			messageAndCommand.set_messagedata(createwebSerializer);
			messageAndCommand.SerializeToString(&serializeString);

		}
		else if (userInput == "AUTHENTICATE")
		{
			std::string AuthenticateSerializer;
			AuthenticateWeb authenticate;
			authenticate.set_email(email);
			authenticate.set_plaintext_password(password);
			authenticate.SerializeToString(&AuthenticateSerializer);

			messageAndCommand.set_command(MessageAndCommand_Command_AUTHENTICATE_WEB);
			messageAndCommand.set_messagedata(AuthenticateSerializer);
			messageAndCommand.SerializeToString(&serializeString);

		}

		std::string message = serializeString;
		int messageLength = message.size();
		int lengthToSend = htonl(messageLength);
		std::vector<uint8_t> sendDataBuffer(sizeof(int) + message.size());
		memcpy(sendDataBuffer.data(), &lengthToSend, sizeof(int));
		memcpy(sendDataBuffer.data() + sizeof(int), message.data(), message.size());

		// Send the entire buffer
		int bytesToSend =send(sock, reinterpret_cast<char*>(sendDataBuffer.data()), sendDataBuffer.size(), 0);
		if (bytesToSend == SOCKET_ERROR)
		{
			std::cerr << "Send failed." << std::endl;
			break;
		}

	}
	

}
