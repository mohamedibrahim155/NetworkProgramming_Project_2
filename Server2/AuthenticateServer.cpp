#include "AuthenticateServer.h"


AuthenticateServer::AuthenticateServer()
{
}

AuthenticateServer::~AuthenticateServer()
{
}

void AuthenticateServer::Initialize()
{
    // Initialize Winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    WSAStartup(ver, &wsData);

    // Create socket
    SOCKET server2Socket = socket(AF_INET, SOCK_STREAM, 0);

    // Bind the socket to an IP address and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54001); // Assuming Server2 is on a different port
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(server2Socket, (sockaddr*)&hint, sizeof(hint));

    // Tell Winsock the socket is for listening
    listen(server2Socket, SOMAXCONN);

    // Wait for a connection
    sockaddr_in server1;
    int server1Size = sizeof(server1);

    SOCKET server1Socket = accept(server2Socket, (sockaddr*)&server1, &server1Size);

    // Receive message from Server1
    char buf[4096];
    ZeroMemory(buf, sizeof(buf));
    recv(server1Socket, buf, sizeof(buf), 0);
    std::cout << "Received from Server1: " << buf << std::endl;

    // Send a completion message to Server1
    const char* completionMessage = "Completed";
    send(server1Socket, completionMessage, strlen(completionMessage) + 1, 0);

    // Close connection to Server1
    closesocket(server1Socket);

    // Cleanup Winsock
    WSACleanup();

}
