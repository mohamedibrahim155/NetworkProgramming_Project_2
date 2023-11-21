
#include "AuthenticateServer.h"
#include "MySQLUtil.h"

int main()
{
	std::cout << "**************************************************" << std::endl;
	std::cout << " AUTHENTICATE SERVICE SERVER" << std::endl;
	std::cout << "**************************************************" << std::endl;

	MySQLUtil database;

	database.ConnectToDatabase("127.0.0.1:3306", "root", "ibrahim");
	
	database.AddPreparedStatement(StatementType::CREATE_ACCOUNT,
		"INSERT INTO gdp.web_auth ( email , salt , hashed_password, userId) VALUES(?, ?, ?, ?) ");

	database.AddPreparedStatement(StatementType::AUTHENTICATE_ACCOUNT,
		"SELECT hashed_password, salt, userId FROM gdp.web_auth WHERE email = ?");

	database.AddPreparedStatement(StatementType::UPDATE_WEB_AUTH_ID,
		"UPDATE gdp.web_auth SET userId = ? WHERE email = ?");

	database.AddPreparedStatement(StatementType::CREATE_USER,
		"INSERT INTO gdp.user (last_login, creation_date) VALUES (NOW(), NOW())");

	database.AddPreparedStatement(StatementType::GET_LAST_INSERT,
		"SELECT LAST_INSERT_ID()");

	database.AddPreparedStatement(StatementType::UPDATE_LAST_LOGIN,
		"UPDATE gdp.user SET last_login = NOW() WHERE id = ?");

	database.AddPreparedStatement(StatementType::GET_CREATION_DATE,
		"SELECT creation_date FROM gdp.user WHERE id = ?");

	AuthenticateServer server;

	server.SetSQLUtil(&database);

	server.Initialize();
	
	return 0;
}