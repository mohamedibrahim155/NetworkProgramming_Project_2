#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include <thread>
#include <iphlpapi.h>
#include <cctype>
#include <algorithm>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#include "../Shared/authentication.pb.h"

class Client
{
public:
	Client();
	~Client();

	void Initialize();
	void CloseClient();

	void ReceiveMessages(SOCKET sock);
	void SendToServer(SOCKET serverSocket);

private:
	SOCKET clientSocket;
};


