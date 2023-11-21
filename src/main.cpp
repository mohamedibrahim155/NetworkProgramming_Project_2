#include <iostream>
#include <fstream>
#include <string>
#include "../Shared/authentication.pb.h"
#include "Client.h"
int main(int argc, char* argv[])
{
	std::cout << "**************************************************" << std::endl;
	std::cout << " CHAT CLIENT" << std::endl;
	std::cout << "**************************************************" << std::endl;
	Client client;
	client.Initialize();

	return 0;
}