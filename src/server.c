#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <strings.h>
#include <string.h>
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
//The server
void error(char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]){
	int sockfd, newsockfd, portno, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	//exe >> port#
	if (argc < 2){
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0){
		error("ERROR opening socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}

	while(1==1) { //while true (runs forever)
		listen(sockfd,5);
		clilen = sizeof(cli_addr);

		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0){
			error("ERROR on accept");
		}

		bzero(buffer,256);

		n = read(newsockfd,buffer,255);

		if (n <= 0){
			error("ERROR reading from socket");
		}
		else {
			char temp[strlen(buffer)];
			strcpy(temp, buffer);
			printf("%s\n",buffer);
			char *token;
			char delim[] = " ";
			token = strtok(temp, delim);  // first call returns pointer to first part of user_input separated by delim
			token = strtok(NULL, delim);  // every call with NULL uses saved user_input value and returns next substring

			//char filename[strlen(token)];
			//strcpy( filename, token);
			char *filename = token;
			filename++;
			//memmove(filename+1, filename+1, strlen(filename-1));
			printf("%s\n",filename);
			//printf("Here is the message: %s\n",buffer);
			//printf("%s\n",buffer);

			n = write(newsockfd,"I got your message",18);

			if (n < 0){
				error("ERROR writing to socket");
			}
		}
	}
	return 0;
}
