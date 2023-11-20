#pragma once

#include<mysql/jdbc.h>


enum class StatementType
{
	CREATE_ACCOUNT = 0,
	AUTHENTICATE_ACCOUNT = 1,
	CREATE_USER = 2,
	UPDATE_WEB_AUTH_ID = 3,
	GET_LAST_INSERT = 4,
	UPDATE_LAST_LOGIN = 5,
	GET_CREATION_DATE = 6,
};


class MySQLUtil
{
public:
	MySQLUtil();
	~MySQLUtil();
	void ConnectToDatabase(const char* host, const char* username, const char* password);
	void Disconnect();
	void AddPreparedStatement(const StatementType& key, const char* query);

	int Update(const char* querry);
	int Insert(const char* querry);

	int AddAccount(const char* email, const char* salt, const char* password);
	int UpdateUserID(const char* email, const int& userID);
	int AuthenticateAccount(const char* email, const char* password, int& userId);


	int AddNewUser(int& userID);
	int UpdateLastLogin(const int& userID, std::string& creationDate);


	sql::ResultSet* Select(const char* querry);
	sql::PreparedStatement* PreparedStatement(const char* querry);
	sql::Connection* GetConnection();
	sql::ResultSet* GetResult();
	sql::Statement* GetStatement();
	sql::PreparedStatement* GetPreparedStatement(const StatementType& key);


	std::map<StatementType, sql::PreparedStatement*> statementsList;

private:

	sql::mysql::MySQL_Driver* driver;
	sql::Connection* connection;
	sql::Statement* statement;
	sql::ResultSet* result;

	bool isConnected;
};


