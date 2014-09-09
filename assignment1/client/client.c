#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define SERVICE_PORT	21235

int main()
{
	int port = SERVICE_PORT;
	char *host = "localhost";
	char recvBuff[1024];
	int n;
	char filename[1000] = "test.txt";
//	filename = "test.txt";

	printf("connecting to %s, port %d\n", host, port);

	struct hostent *hp;	/* host information */
	unsigned int alen;	/* address length when we get the port number */
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in servaddr;	/* server address */
	int fd;  /* fd is the file descriptor for the connected socket */

	/* get a tcp/ip socket */
	/* We do this as we did it for the server */
	/* request the Internet address protocol */
	/* and a reliable 2-way byte stream */

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0){
		printf("Failed to create socket!\n");
		return -1;
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	memset(recvBuff, '0' ,sizeof(recvBuff));
	memset((char*)&servaddr, 0, sizeof(servaddr));

	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		printf("Bind Failed!\n");
		close(fd);
		return -1;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	/* look up the address of the server given its name */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "could not obtain address of %s\n", host);
		close(fd);
		return -1;
	}

	/* put the host's address into the server address structure */
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	/* connect to server */
	if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("Connect failed!\n");
		close(fd);
		return -1;
	}

	write(fd, filename, strlen(filename));

	while((n = read(fd, recvBuff, sizeof(recvBuff)-1)) > 0)
	{
		recvBuff[n] = 0;
		if(fputs(recvBuff, stdout) == EOF){
			printf("\n Error : Fputs error");
		}
		printf("\n");
	}

	if( n < 0){
		printf("\n Read Error \n");
	}
	close(fd);
	return 0;
}
