#include "MySQLUtil.h"
#include "sha256.h"
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

 void MySQLUtil::AddPreparedStatement(const StatementType& key, const char* query)
 {
	 statementsList[key] = connection->prepareStatement(query);
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

 int MySQLUtil::AddAccount(const char* email, const char* salt, const char* password)
 {
	 sql::PreparedStatement* statement = GetPreparedStatement(StatementType::CREATE_ACCOUNT);
	 statement->setString(1, email);
	 statement->setString(2, salt);
	 statement->setString(3, password);
	 statement->setBigInt(4, "45");

	 try
	 {
		 statement->execute();
		 std::cout << "Account added successfully!" << std::endl;
		 return 1;
	 }
	 catch (sql::SQLException& e)
	 {
		 if (e.getErrorCode() == 1062)
		 {
			 std::cout << "Error: Email address already exists." << std::endl;

			 return 0;
		 }
		 else
		 {
			 std::cout << "SQL Error: " << e.what() << std::endl;
			 return -1;
		 }
		 return -1;
	 }
	
 }

 int MySQLUtil::UpdateUserID(const char* email, const int& userID)
 {
	 sql::PreparedStatement* statement = GetPreparedStatement(StatementType::UPDATE_WEB_AUTH_ID);
	 statement->setBigInt(1, std::to_string(userID));
	 statement->setString(2, email);

	 try
	 {
		 statement->execute();

		 std::cout << "User ID updated successfully!" << std::endl;

		 return 1;
	 }
	 catch (sql::SQLException& e)
	 {

		 std::cout << "SQL Error: " << e.what() << std::endl;
		 return -1;
	 }
 }

 int MySQLUtil::AuthenticateAccount(const char* email, const char* password, int& userId)
 {
	 sql::PreparedStatement* stmt =GetPreparedStatement(StatementType::AUTHENTICATE_ACCOUNT);
	 stmt->setString(1, email);


	 try
	 {
		 sql::ResultSet* result = stmt->executeQuery();

		 if (result->next())
		 {

			 SHA256 hasher;

			 std::string passwordDB = result->getString("hashed_password");
			 std::string salt = result->getString("salt");
			 userId = result->getInt("userId");

			 std::string hashedPassword = hasher(salt + password);
			 //std::string hashedPassword = password;

			 if (passwordDB == hashedPassword)
			 {
				 std::cout << "Valid Password" << std::endl;
				 return 1;
			 }
			 else
			 {
				 std::cout << "Invalid Password" << std::endl;
				 return 2;
			 }
		 }
		 else
		 {
			 std::cout << "Email ID not in database " << std::endl;
			 return 0;
		 }
	 }
	 catch (sql::SQLException& e)
	 {
		 std::cout << "SQL Error: " << e.what() << std::endl;
		 return -1;
	 }
 }

 int MySQLUtil::AddNewUser(int& userID)
 {
	 sql::PreparedStatement* stmt = GetPreparedStatement(StatementType::CREATE_USER);

	 try
	 {
		 stmt->execute();

		 sql::ResultSet* resultUser = GetPreparedStatement(StatementType::GET_LAST_INSERT)->executeQuery();

		 while (resultUser->next())
		 {
			 userID = resultUser->getInt(1);
		 }

		 std::cout << "User Account added successfully!" << std::endl;

		 return 1;
	 }
	 catch (sql::SQLException& e)
	 {

		 std::cout << "SQL Error: " << e.what() << std::endl;
		 return -1;
	 }
 }

 int MySQLUtil::UpdateLastLogin(const int& userID, std::string& creationDate)
 {
	 sql::PreparedStatement* stmt = GetPreparedStatement(StatementType::UPDATE_LAST_LOGIN);
	 sql::PreparedStatement* stmt2 = GetPreparedStatement(StatementType::GET_CREATION_DATE);

	 stmt->setInt(1, userID);
	 stmt2->setInt(1, userID);

	 try
	 {
		 stmt->execute();

		 sql::ResultSet* result = stmt2->executeQuery();


		 if (result->next())
		 {
			 creationDate = result->getString("creation_date");
		 }
		 std::cout << "Updated Last Login Successfully " << std::endl;

		 return 1;
	 }
	 catch (sql::SQLException& e)
	 {

		 std::cout << "SQL Error: " << e.what() << std::endl;
		 return -1;
	 }

	 return 0;
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

 sql::PreparedStatement* MySQLUtil::GetPreparedStatement(const StatementType& key)
 {
	 return statementsList[key];
 }
