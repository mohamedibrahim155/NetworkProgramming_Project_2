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
	sql::PreparedStatement* PreparedStatement(const char* querry);
	int Update(const char* querry);
	int Insert(const char* querry);

	sql::Connection* GetConnection();
	sql::ResultSet* GetResult();
	sql::Statement* GetStatement();

private:

	sql::mysql::MySQL_Driver* driver;
	sql::Connection* connection;

	sql::Statement* statement;
	sql::ResultSet* result;

	bool isConnected;
};


