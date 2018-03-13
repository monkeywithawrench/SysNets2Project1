#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <strings.h>        //Strings Library
#include <string.h>         //String Library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <unistd.h>         //Needed for access() to check file information


#define BUFFER_MAX_SIZE 2048 //Defining a constant for max buffer size. Cleaner than magic numbers
							 //Plus 256 isn't enough bytes anyways

//The server
int main(int argc, char *argv[]){
	
	struct sockaddr_in server_address, client_address;
	int socket_hold, newsocket, port, client, n;
	char buffer[BUFFER_MAX_SIZE];

	//exe >> port#
	if (argc < 2){
		fprintf(stderr,"You haven't provided a port number\n");
		exit(1);
	}
	//Check socket success
	socket_hold = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_hold < 0){
		fprintf(stderr,"Opening socket failed");
      		exit(1);
	}

	//Start socketing process
	memset((char *) &server_address,0,sizeof(server_address));
	port = atoi(argv[1]);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port);

	//Check binding success
	if (bind(socket_hold, (struct sockaddr *) &server_address,sizeof(server_address)) < 0){//the socket, its cast, the size
		fprintf(stderr,"Binding failed");
     	 	exit(1);
	}

	while(1==1) { //while true (runs forever)
		listen(socket_hold,5);//(reference to file descriptor, max queue
		client = sizeof(client_address);

		newsocket = accept(socket_hold,(struct sockaddr *) &client_address, &client);//(file descriptor socketed-binded-listened,pointer to sockaddr struct, sizeof pointed struct)

		if (newsocket < 0){
			fprintf(stderr,"Accept failed");
         		exit(1);
		}

		memset(buffer,0,BUFFER_MAX_SIZE);
		n = read(newsocket,buffer,BUFFER_MAX_SIZE);

		if (n <= 0){
			fprintf(stderr,"Reading from socket fail");
         		exit(1);
		}
		else {
			char temp[strlen(buffer)+1];
			strcpy(temp, buffer);//placeholder for buffer in temp
			printf("%s\n",buffer);
			char *token;		      
			char delim[2] = " ";
			token = strtok(temp, delim);  // first call returns pointer to first part of user_input separated by delim
			/*TODO: seg fault, no space in string triggers it when sending a message*/
			token = strtok(NULL, delim);  // every call with NULL uses saved user_input value and returns next substring
			token++;
			char filename[strlen(token)];
			strcpy( filename, token);
			//printf("%s\n",filename);
			//printf("Here is the message: %s\n",buffer);
			//printf("%s\n",buffer);


			if(strlen(filename)>0) {
				char *fileLocation;
				asprintf(&fileLocation, "./%s", filename);
				if(access(fileLocation, F_OK) != -1) {  //F_OK checks if file exists, R_OK checks if file can be read
					//file exists

					char *postrequest;
					//asprintf(&postrequest, "POST /%s HTTP/1.1\n", filename);
					asprintf(&postrequest, "HTTP/1.1 200 OK\n");
					//asprintf(&postrequest, "%sHost: notarealaddress\n", postrequest);
					//asprintf(&postrequest, "%sConnection: keep-alive\n");
					//asprintf(&postrequest, "%sConnection: close\n", postrequest);
					asprintf(&postrequest, "%sContent-Length: 44\n", postrequest);
					//asprintf(&postrequest, "%sCache-Control: no-cache\n", postrequest);
					//asprintf(&postrequest, "%sOrigin: Server program info\n", postrequest);
					//asprintf(&postrequest, "%sUser-Agent: Server machine info\n", postrequest);
					asprintf(&postrequest, "%sContent-Type: text/html\n", postrequest);
					asprintf(&postrequest, "%s\n<html><body><h1>It works!</h1></body></html>\n", postrequest);

					n = write(newsocket,postrequest, BUFFER_MAX_SIZE);
				}
			}

			//n = write(newsocket,"Message is up, thanks\n",BUFFER_MAX_SIZE);

			if (n < 0){
				fprintf(stderr,"Writing to socket fail");
            			exit(1);
			}
		}
	}
	return 0;
}
