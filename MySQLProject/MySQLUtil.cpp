#include "MySQLUtil.h"

MySQLUtil::MySQLUtil() :driver(nullptr), isConnected(false), statement(nullptr), result(nullptr)
{
}

MySQLUtil::~MySQLUtil()
{
	if (connection!=nullptr)
	{
		delete connection;
	}

	if (result!=nullptr)
	{
		delete result;
	}
	if (statement!=nullptr)
	{
		delete statement;
	}
}

 void MySQLUtil::ConnectToDatabase(const char* host, const char* username, const char* password)
{
	 if (isConnected)
	 {
		 return;
	 }

	 try
	 {
		 driver = sql::mysql::get_mysql_driver_instance();
		 connection = driver->connect(host, username, password);
		 statement = connection->createStatement();
		 connection->setSchema("gdp");
	 }
	 catch (sql::SQLException &ex)
	 {

		 std::cout << "#ERROR : " << ex.what() << std::endl;
		 std::cout << "MYSQL error Code : " << ex.getErrorCode() << std::endl;
		 std::cout << "SQL state  : " << ex.getSQLState() << std::endl;
		 std::cout << "Failed to connect" << std::endl;
	 }

	 isConnected = true;
	 std::cout << "Successfully connected to MySQL database.. " << std::endl;
}

 void  MySQLUtil::Disconnect()
 {
	 if (!isConnected)
	 {
		 return;
	 }
	 connection->close();
	 isConnected = false;
 }

 sql::ResultSet* MySQLUtil::Select(const char* querry)
 {
	 result = statement->executeQuery(querry);
	 return result;

 }

 sql::PreparedStatement* MySQLUtil::PreparedStatement(const char* querry)
 {
	 return GetConnection()->prepareStatement(querry);
 }

 int MySQLUtil::Update(const char* querry)
 {
	 int update = statement->executeUpdate(querry);
	 return update;
 }

 int MySQLUtil::Insert(const char* querry)
 {
	 int insert = statement->executeUpdate(querry);
	 return insert;
 }

 sql::Connection* MySQLUtil::GetConnection()
 {
	 return connection;
 }

 sql::ResultSet* MySQLUtil::GetResult()
 {
	 return result;
 }

 sql::Statement* MySQLUtil::GetStatement()
 {
	 return statement;
 }
