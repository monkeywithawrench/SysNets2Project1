#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <strings.h>        //Strings Library
#include <string.h>         //String Library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <netdb.h>


#define BUFFER_MAX_SIZE 2048 //Defining a constant for max buffer size. Cleaner than magic numbers
							 //Plus 256 isn't enough bytes anyways

//The client
int main(int argc, char *argv[]){

	struct sockaddr_in server_address;//IPV4
	struct hostent *server;//store info of host
	int socket_hold, port, n;
	char buffer[BUFFER_MAX_SIZE];

	//exe >> purpose >> port#
	if (argc < 3){
		fprintf(stderr,"Please provide a hostname and port number\n");
		exit(0);
	}

	//Check for valid port 
	port = atoi(argv[2]);
        /*if(portno < 60001 || portno > 60099){    
		fprintf(stderr,"Invalid port number");
		exit(0);
	}*/

	//Check for socket
	socket_hold = socket(AF_INET, SOCK_STREAM, 0);//return file descriptor, else -1
	if (socket_hold < 0 ){
		fprintf(stderr,"Socket Fail");
      		exit(0);
	}

	//Check hostname
	server = gethostbyname(argv[1]);//server name, return hostent
	if (server == NULL){
		fprintf(stderr,"Host does not exist\n");
		exit(0);
	}
	
	memset((char*) &server_address,0,sizeof(server_address));
	server_address.sin_family = AF_INET;//For internet, IP address, address family
	memmove((char*) &server_address.sin_addr.s_addr,(char*)server->h_addr,server->h_length);
	server_address.sin_port = htons(port);
	
	//Check the connection
	int checkConnect = connect(socket_hold,(struct sockaddr *)&server_address,sizeof(server_address));//(reference to socket by file descriptor,the specified address, address space of socket)
	if ( checkConnect < 0){
		fprintf(stderr,"Failed connectiopn");
      		exit(0);
	}

	/*Interface with the user*/
	//printf("Enter message sent to server: ");
	printf("Enter requested file name: ");
	memset(buffer,0,BUFFER_MAX_SIZE);
	fgets(buffer,BUFFER_MAX_SIZE,stdin);

	char *getrequest;
	asprintf(&getrequest, "POST /%s HTTP/1.1\n", buffer);
	asprintf(&getrequest, "%sHost: %s\n", getrequest, argv[1]);
	asprintf(&getrequest, "%sConnection: keep-alive\n");
	//asprintf(&getrequest, "%sConnection: close\n", getrequest);
	asprintf(&getrequest, "%sContent-Length: 44\n", getrequest);
	asprintf(&getrequest, "%sCache-Control: no-cache\n", getrequest);
	asprintf(&getrequest, "%sOrigin: Server program info\n", getrequest);
	asprintf(&getrequest, "%sUser-Agent: Server machine info\n", getrequest);
	asprintf(&getrequest, "%sContent-Type: text/html\n", getrequest);
	asprintf(&getrequest, "%s<html><body><h1>It works!</h1></body></html>\n", getrequest);

	n = write(socket_hold,getrequest,strlen(getrequest));//(reference to socket by file descriptor, the message written, write up to this length

	//n = write(socket_hold,buffer,strlen(buffer));//(reference to socket by file descriptor, the message written, write up to this length
	//Check write success
	if (n < 0){
		fprintf(stderr,"Writing to socket fail");
      		exit(0);
	}
	memset(buffer,0,BUFFER_MAX_SIZE);
	//Check read success
	n = read(socket_hold,buffer,BUFFER_MAX_SIZE);//(reference to socket by file descriptor, the message read, read up to this length)
	if (n < 0){
		fprintf(stderr,"Reading from socket fail");
      		exit(0);
	}
	printf("%s\n",buffer);

	return 0;
}
