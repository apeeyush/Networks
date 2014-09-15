Steps To Run:

Compile server1.c, server2.c and client.c present in server1, server2 and client folders respectively.
You can use Makefile by using following commands if you are in assignment1 folder
	make
Or, you can use following commands if you are in assignment1 folder:
	gcc server1/server1.c -o server1/server
	gcc server2/server2.c -o server2/server
	gcc client/client.c -o client/client


Now, open 3 terminal windows. Change directory to server1, server2, client respectvely in the 3 terminal windows.
You can use following commands if you are in assignment1 folder:
	cd server1/
	cd server2/
	cd server3/


Now, on the terminal windows with server directory, run ./server to start server.
Input the port number. You can use the suggested value or enter your own value.

On client, use ./client to start the client.
Input ports for the two servers and filename to be transferred from the servers.


Note
Files present in server1 folder are assumed to belong to server1. They will be detected and transferred if the executable for server1 is run from server1 folder i.e. 
	./server
command is run in directory server1/
Similar is the case with server2 and client.