#pragma once
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
#define DEFAULT_PORT "5001"
#define DEFAULT_BUFLEN 512

#include "../Shared/authentication.pb.h"
#include"MySQLUtil.h"
class AuthenticateServer
{
public:
	AuthenticateServer();
	~AuthenticateServer();
	void Initialize();

	void AcceptClientConnections(SOCKET serverSocket);
	void ServerClose();
	void ReceiveAndPrintIncomingMessageOnSeparateThread(SOCKET clientSocket);
	void ReceiveAndPrintIncomingMessage(SOCKET clientSocket);
	void SendMessagestoServer1(MessageAndCommand messagetoSend,SOCKET server1Scoket);

	void SetSQLUtil(MySQLUtil* mysql);
private:
	SOCKET serverSocket;
	std::vector<SOCKET> clientSockets;
	std::vector<std::thread> clientThreads;
	MySQLUtil* mysql;
};

