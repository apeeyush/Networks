#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define SERVER1_PORT	21235
#define SERVER2_PORT	21236

#define LENGTH 100000

int server_rcv(char filename[1000], int port){
	char *host = "localhost";
	char recvBuff[LENGTH];
	int flag = -1;

	printf("Connecting to %s, port %d\n", host, port);

	struct hostent *hp;
	struct sockaddr_in clientsockaddr;
	struct sockaddr_in serversockaddr;
	int server_fd;

	memset((char *)&clientsockaddr, 0, sizeof(clientsockaddr));
	memset((char*)&serversockaddr, 0, sizeof(serversockaddr));
	memset(recvBuff, '0' ,sizeof(recvBuff));

	clientsockaddr.sin_family = AF_INET;
	clientsockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientsockaddr.sin_port = htons(0);

	serversockaddr.sin_family = AF_INET;
	serversockaddr.sin_port = htons(port);

	hp = gethostbyname(host);
	if (!hp) {
		printf("Could not obtain address of %s\n", host);
		close(server_fd);
		return flag;
	}
	memcpy((void *)&serversockaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0){
		printf("Failed to create socket!\n");
		return flag;
	}
	if (bind(server_fd, (struct sockaddr *)&clientsockaddr, sizeof(clientsockaddr)) < 0) {
		printf("Bind Failed!\n");
		close(server_fd);
		return flag;
	}
	if (connect(server_fd, (struct sockaddr *)&serversockaddr, sizeof(serversockaddr)) < 0) {
		printf("Connect failed!\n");
		close(server_fd);
		return flag;
	}
	write(server_fd, filename, strlen(filename));
	bzero(recvBuff, LENGTH); 
	int fr_block_sz = 0;
	if ( (fr_block_sz = read(server_fd, recvBuff, LENGTH)) <=0){
		printf("File not found on Server with port %d.\n", port);
	}else{
		FILE *fr = fopen(filename, "a");
		if(fr == NULL)
			printf("File %s Cannot be opened.\n", filename);
		else{
			int write_sz = fwrite(recvBuff, sizeof(char), fr_block_sz, fr);
			if(write_sz < fr_block_sz){
				printf("File write failed.\n");
			}
			bzero(recvBuff, LENGTH);
			while((fr_block_sz = read(server_fd, recvBuff, LENGTH)) > 0){
				int write_sz = fwrite(recvBuff, sizeof(char), fr_block_sz, fr);
				if(write_sz < fr_block_sz){
					printf("File write failed.\n");
				}
				bzero(recvBuff, LENGTH);
				if (fr_block_sz == 0 || fr_block_sz != 512){
					break;
				}
			}
			if(fr_block_sz < 0){
				printf("File transfer failed!\n");
			}
			printf("Congratulations! File received from server with port %d!\n", port);
			flag = 1;
			fclose(fr);
		}
	}
	close(server_fd);
	return flag;
}

int main(){
	char *host = "localhost";
	char filename[1000];
	int status;

	printf("Enter filename:");
	scanf("%s", filename);

	status = server_rcv(filename, SERVER1_PORT);
	if (status == -1){
		server_rcv(filename, SERVER2_PORT);
	}
	return 0;
}
