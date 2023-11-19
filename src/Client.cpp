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

    CreateAccountWeb createWeb;
    createWeb.set_email("MohamedIbrahim@gmail.com");
    createWeb.set_plaintext_password("mohamed");

    std::string serializeCreateWeb;

    createWeb.SerializeToString(&serializeCreateWeb);

   
   
    std::string message = serializeCreateWeb;

    int messageLength = message.size();
    int lengthToSend = htonl(messageLength);
    std::vector<uint8_t> sendDataBuffer(sizeof(int) + message.size());
    memcpy(sendDataBuffer.data(), &lengthToSend, sizeof(int));
    memcpy(sendDataBuffer.data() + sizeof(int), message.data(), message.size());

    // Send the entire buffer
    send(clientSocket, reinterpret_cast<char*>(sendDataBuffer.data()), sendDataBuffer.size(), 0);


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
