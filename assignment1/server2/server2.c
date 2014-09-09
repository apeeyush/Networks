#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/types.h>

#define SERVICE_PORT	21234
#define LENGTH 1025

int main()
{
	int port = SERVICE_PORT;

	int svc;        /* listening socket providing service */
	int rqst;       /* socket accepting the request */
	socklen_t alen;       /* length of address structure */
	struct sockaddr_in my_addr;    /* address of this service */
	struct sockaddr_in client_addr;  /* client's address */
	int sockoptval = 1;
	char hostname[128]; /* host name, for debugging */
	char sendBuff[LENGTH];
	char filename[1000];
	int rc, fd;

	gethostname(hostname, 128);

	svc = socket(AF_INET, SOCK_STREAM, 0);
	if (svc < 0){
		printf("Failed to create socket!\n");
		return -1;
	}

	/* 0 out the structure */
	memset( (char*)&my_addr, 0, sizeof(my_addr));
	memset(sendBuff, '0', sizeof(sendBuff));

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind to the address to which the service will be offered */
	if (bind(svc, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		printf("Bind Failed!\n");
		return -1;
	}

	if (listen(svc, 10) < 0) {
		printf("Failed to listen!\n");
		return -1;
	}

	printf("Congratulations! Server started on %s. Listening on port %d\n", hostname, port);

	int sin_size = sizeof(client_addr);

	while(1){
		rqst = accept(svc, (struct sockaddr *)&client_addr, &sin_size);
		if (rqst < 0){
			printf("Accept Failed!\n");
			return -1;
		}
		rc = recv(rqst, filename, sizeof(filename), 0);
		if (rc == -1){
			printf("Filename Recieve Failed!\n");
			return -1;
		}
		/* null terminate and strip any \r and \n from filename */
		filename[rc] = '\0';
		if (filename[strlen(filename)-1] == '\n')
			filename[strlen(filename)-1] = '\0';
		if (filename[strlen(filename)-1] == '\r')
			filename[strlen(filename)-1] = '\0';

		printf("Received a connection from: %s port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		FILE *fs = fopen(filename,"r");
		if (fs == NULL) {
			printf("Unable to open '%s'\n", filename);
			return -1;
		}
		printf("Congratulations! Found '%s' on Server 1\n", filename);
		int fs_block_sz;
		while((fs_block_sz = fread(sendBuff, sizeof(char), LENGTH, fs))>0){
			if(send(rqst, sendBuff, fs_block_sz, 0) < 0){
				printf("Failed to send file %s.\n", filename);
				return -1;
			}
			bzero(sendBuff, LENGTH);
		}
		printf("File transfer successful!");
		close(rqst);
		sleep(1);
	}
	return 0;
}
