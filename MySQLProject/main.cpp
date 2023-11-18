
#include "MySQLUtil.h"
int main()
{
	MySQLUtil database;

	database.ConnectToDatabase("127.0.0.1:3306", "root", "ibrahim");

	sql::ResultSet* result = database.Select("SELECT * FROM user;");

	while (result->next())
	{
		sql::SQLString id = result->getString("id");
		std::cout << "ID : " << id << std::endl;
	}
	return 0;
}