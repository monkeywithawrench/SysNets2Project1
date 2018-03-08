#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <strings.h>        //Strings Library
#include <string.h>         //String Library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <netdb.h>

//The client
int main(int argc, char *argv[])
{
	int socket_hold, port, n;
	struct sockaddr_in server_address;
	struct hostent *server;
	
	char buffer[256];
	//exe >> purpose >> port#
	if (argc < 3){
		fprintf(stderr,"Please provide a hostname and port number\n");
		exit(0);
	}

	port = atoi(argv[2]);
   
   /*if(portno < 60001 || portno > 60099){    //Check for valid port 
		fprintf(stderr,"Invalid port number");
		exit(0);
	}*/

	socket_hold = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_hold < 0 ){
		fprintf(stderr,"Socket Fail");
      		exit(0);
	}
	server = gethostbyname(argv[1]);//server name
	if (server == NULL){
		fprintf(stderr,"Host don't exist\n");
		exit(0);
	}
	
	//int addrSize = sizeof(server_address);
	//char * addr = (char*) &server_address;
	bzero((char*) &server_address,sizeof(server_address));//clear adress
	server_address.sin_family = AF_INET;

	bcopy((char*)server->h_addr,(char*) &server_address.sin_addr.s_addr,server->h_length);
	server_address.sin_port = htons(port);
	
	int checkConnect = connect(socket_hold,(struct sockaddr *)&server_address,sizeof(server_address));
	if ( checkConnect < 0){
		fprintf(stderr,"Failed connectiopn");
      		exit(0);
	}
	printf("Enter message sent to server: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	
	n = write(socket_hold,buffer,strlen(buffer));
	if (n < 0)
	{
		fprintf(stderr,"Writing to socket fail");
      		exit(0);
	}
	bzero(buffer,256);
	n = read(socket_hold,buffer,255);
	if (n < 0){
		fprintf(stderr,"Reading from socket fail");
      		exit(0);
	}
	printf("%s\n",buffer);

	return 0;
}
