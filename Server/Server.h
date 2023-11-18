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
class Server
{
public:
	Server();
	~Server();

	void Initialize();
	void ServerClose();
	void AcceptClientConnections();

	void ReceiveAndPrintIncomingMessage(SOCKET clientSocket);
	void ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET clientSocket);
  
	




private:
	SOCKET serverSocket;
	std::vector<SOCKET> clientSockets;
	std::vector<std::thread> clientThreads;

};



