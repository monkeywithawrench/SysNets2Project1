#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <strings.h>
#include <string.h>
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <netdb.h>

void error(char *msg){
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	char buffer[256];
	//exe >> purpose >> port#
	if (argc < 3){
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0 ){
		error("Socket Fail");
	}
	server = gethostbyname(argv[1]);//server name
	if (server == NULL){
		fprintf(stderr,"Invalid Host\n");
		exit(0);
	}
	
	//int addrSize = sizeof(serv_addr);
	//char * addr = (char*) &serv_addr;
	bzero((char*) &serv_addr,sizeof(serv_addr));//clear adress
	serv_addr.sin_family = AF_INET;

	bcopy((char*)server->h_addr,(char*) &serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	
	int checkConnect = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	if ( checkConnect < 0){
		error("ERROR connecting");
	}
	printf("Please enter the message: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0)
	{
		error("ERROR writing to socket");
	}
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0){
		error("ERROR reading from socket");
	}
	printf("%s\n",buffer);

	return 0;
}
