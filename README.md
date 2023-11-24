# NetworkProgramming_Project_2
Network Programming Project_2

******************************************************************************************************************
Build Instruction:

* Open  Network_Programming_Project2.sln solution
* Set Configuration as "RELEASE".

Build steps :
Step 1 - Build "Server2" project
Step 2 - Build "Server1" project
Step 3 - Build "Client" project
******************************************************************************************************************

Working Flow: 
1. Open Server2.exe (This is authenticate server which involves SQL)
2. Open Server.exe  (This is Client Server)
3. Open Client (This you can open multiple instances)

4. Type your command in capital letters as "REGISTER" or "AUTHENTICATE"
5. Type your email
6. Type your password

******************************************************************************************************************



Questions attended :

1 . Authenticate Protocol :
       * Created authencation protocols using Google protocol Buffers. (Open  projectDir/Shared/ authencation.proto file)
       * Using authencation.proto for the serialization and Deserialization in both authencation server and Client server.

2. Authentication Server Database:

        * Created Database according to requirement using MYSQL workbench.
		* To check: Open ProjectDir/Database file -> gdp_user and gdp_web_auth.
		
3. Authencation Service (Open "Server2" Project) :
		Create account :
	    * Used TCP client 
		* Used Length prefix serialization (for eg. in send request: Client.cpp -> Line no: (266 -274)/ in receive request Client.cpp -> line No: (122-223)
		* User can create a new account (AuthencateServer.cpp -> Line no: 205)
		* Using Hash, salt  and adding to SQL (AuthencateServer.cpp -> Line no: 201-205)
		* Response sending succes or failure(AuthencateServer.cpp -> Line no: 206-240)
		
		 Authenticate account :
		* Hashing and comparing with Database (AuthencateServer.cpp-> Line no: 260)
		* Responding success or failure (AuthencateServer.cpp-> Line no: 261-297)
		* Updating last login (AuthencateServer.cpp -> Line no: 263(updateLastlogin()))
		
4. Authentication Client (Open "Server" project)
        * using TCP client  for "server" project
		* Using length prefix serialization in this project
		* Connection established with AuthencateServer (Server.cpp ->line no: 112 )
		* receive messages from client to create or authenticate User
		
5. Chat Client (Open "Client" project)
		* Can send REGISTER and AUTHENTICATE 
		* Prints if success with creation date and failure with reasons.
		* Check functions receivemessages() and SendToServer() in (Client.cpp)
		
******************************************************************************************************************