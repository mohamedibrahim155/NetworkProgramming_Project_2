#pragma once
// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include<thread>
#include <iphlpapi.h>

#include<map>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <cctype>
#include <algorithm>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#include <../Shared/authentication.pb.h>

struct Client
{
	int requestId;
	SOCKET clientSocket;
};

class Server
{
public:
	Server();
	~Server();

	void Initialize();
	void ServerClose();
	void AcceptClientConnections(SOCKET serverSocket);

	void ReceiveMessagesFromClient(Client* client);
	void ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET clientSocket);
	void SendMessagestoClient(MessageAndCommand messagetobeSend, Client* client);
    
	void SendMessageToAuthenticateServer(MessageAndCommand messagetobeSend, SOCKET server2Scoket);
	void ReceiveMessagesFromAuthenticateServer(SOCKET sock);

	Client* GetClientWithRequestID(int requestId);


private:
	SOCKET serverSocket;
	SOCKET server2Socket;
	std::vector<SOCKET> clientSockets;
	std::vector<Client*> clientList;
	std::vector<std::thread> clientThreads;

	int clientRequestId;
};




