#include <iostream>
#include <fstream>
#include <string>
#include "../Shared/authentication.pb.h"

int main(int argc, char* argv[])
{
	CreateAccountWeb web;
	web.set_request_id(1);
	web.set_email("Mohamed@gmail.com");
	web.set_plaintext_password("MypassWord");

	std::string parsedData;
	web.SerializeToString(&parsedData);
	
    
	std::cout << "Paresed Data : " << parsedData << std::endl;
	
	CreateAccountWeb webReq;

	bool result =webReq.ParseFromString(parsedData);
	if (result)
	{
		std::cout << webReq.request_id() << std::endl;
		std::cout << webReq.email()<<std::endl;
		std::cout << webReq.plaintext_password() << std::endl;
	}

	return 0;
}