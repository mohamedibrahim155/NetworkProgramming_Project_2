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
    WORD ver = MAKEWORD(2, 2);
    WSAStartup(ver, &wsData);

    // Create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Connect to Server1
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    connect(clientSocket, (sockaddr*)&hint, sizeof(hint));

   

    std::string serializeString;
    std::string userInput,email,password;
    std::cout << "ENTER COMMAND REGISTER OR AUTHENTICATE : ";
    std::cin >> userInput;
    std::cout << "ENTER EMAIL : ";
    std::cin >> email;  
    std::cout << "ENTER PASSWORD : ";
    std::cin >> password;

    MessageAndCommand messageAndCommand;
    if (userInput =="REGISTER")
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
    else
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
    send(clientSocket, reinterpret_cast<char*>(sendDataBuffer.data()), sendDataBuffer.size(), 0);




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
			CreateAccountWebSuccess CreateAccountWebSuccess;
			switch (messageAndCommand.command())
			{

			case MessageAndCommand_Command_CREATE_ACCOUNT_WEB:
				CreatewebDeserializer.ParseFromString(messageAndCommand.messagedata());
				std::cout << "EMAIL : " << CreatewebDeserializer.email() << std::endl;
				std::cout << "PASSWORD : " << CreatewebDeserializer.plaintext_password() << std::endl;
				break;
			case MessageAndCommand_Command_CREATE_ACCOUNT_WEB_SUCCESS:
				CreateAccountWebSuccess.ParseFromString(messageAndCommand.messagedata());
				std::cout << "REQUEST ID : " << CreateAccountWebSuccess.request_id() << std::endl;
				std::cout << "USER ID : " << CreateAccountWebSuccess.user_id() << std::endl;
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

		}
	}

    // Receive a message from Server1
    char buf[4096];
    ZeroMemory(buf, sizeof(buf));
    recv(clientSocket, buf, sizeof(buf), 0);
    std::cout << "Received from Server1: " << buf << std::endl;
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
	char buf[4096];
	ZeroMemory(buf, sizeof(buf));
	
	while (true)
	{
		int bytesReceived = recv(clientSocket, buf, sizeof(buf), 0);
		std::cout << "Received from Server1: " << buf << std::endl;

	}
}

void Client::SendToServer(SOCKET serverSocket)
{
	std::string userInput;
	std::cout << "Enter your Message : " << std::endl;
	std::cin >> userInput;
	char buf[4096];
	const char* message = userInput.c_str();
    while (true)
    {
        int sendResult = send(serverSocket, message, strlen(message) + 1, 0);
        if (sendResult == SOCKET_ERROR)
        {
            std::cerr << "Send failed." << std::endl;
            break;
        }

    }
}
