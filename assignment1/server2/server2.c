#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/types.h>

#define LENGTH 100000

int main()
{
	int port;
	int svc;
	int rqst;
	struct sockaddr_in server_addr, client_addr;
	int sockoptval = 1;
	char hostname[128];
	char buff[LENGTH];
	char filename[1000];
	int rc, fd;

	gethostname(hostname, 128);

	printf("Enter port value (Eg. 21497):");
	scanf("%d",&port);

	svc = socket(AF_INET, SOCK_STREAM, 0);
	if (svc < 0){
		printf("Failed to create socket!\n");
		return -1;
	}

	/* 0 out the structure */
	memset( (char*)&server_addr, 0, sizeof(server_addr));
	memset(buff, '0', sizeof(buff));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind to the address to which the service will be offered */
	if (bind(svc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Bind Failed!\n");
		return -1;
	}

	if (listen(svc, 10) < 0) {
		printf("Failed to listen!\n");
		return -1;
	}

	printf("Server 2 started on %s. Listening on port %d\n", hostname, port);

	int sin_size = sizeof(client_addr);

	while(1){
		rqst = accept(svc, (struct sockaddr *)&client_addr, &sin_size);
		if (rqst < 0){
			printf("Accept Failed!\n");
			continue;
		}
		rc = read(rqst, filename, sizeof(filename));
		if (rc == -1){
			printf("Filename Recieve Failed!\n");
			close(rqst);
			continue;
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
			printf("File '%s' not found on server 2.\n", filename);
			close(rqst);
			continue;
		}
		printf("Congratulations! Found '%s' on Server 2\n", filename);
		int block_size;
		while((block_size = fread(buff, sizeof(char), LENGTH, fs))>0){
			sleep(1);											// Sleep prevents the bytes from getting mixed as write operation takes place
			if(write(rqst, buff, block_size) < 0){
				printf("Failed to send file %s.\n", filename);
				continue;
			}
			bzero(buff, LENGTH);
		}
		printf("File transfer successful!\n");
		close(rqst);
	}
	return 0;
}
