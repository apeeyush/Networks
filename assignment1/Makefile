all: server1/server1.c server2/server2.c client/client
	gcc server1/server1.c -o server1/server
	gcc server2/server2.c -o server2/server
	gcc client/client.c -o client/client

clean:
	rm -f server1/server
	rm -f server2/server
	rm -f client/client