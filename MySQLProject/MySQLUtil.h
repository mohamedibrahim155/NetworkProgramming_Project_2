#pragma once

#include<mysql/jdbc.h>

class MySQLUtil
{
public:
	MySQLUtil();
	~MySQLUtil();
	void ConnectToDatabase(const char* host, const char* username, const char* password);
	void Disconnect();

	sql::ResultSet* Select(const char* querry);

private:

	sql::mysql::MySQL_Driver* driver;
	sql::Connection* connection;

	sql::Statement* statement;
	sql::ResultSet* result;

	bool isConnected;
};


