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

char* readFile(char *fileLocation, char *readMode);
long getFileSize(char *fileLocation);

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
			strcpy( filename, token); //filename now saved
			strcpy(delim,"."); //going for the file extension now
			strcpy(temp, filename);
			token = strtok(temp, delim);
			token = strtok(NULL, delim);
			char fileExtension[strlen(token)];
			strcpy(fileExtension, token);

			//printf("%s\n",filename);
			//printf("Here is the message: %s\n",buffer);
			//printf("%s\n",buffer);


			if(strlen(filename)>0 && strlen(fileExtension)>0) {
				char *fileLocation;
				asprintf(&fileLocation, "./%s", filename);
				if(access(fileLocation, R_OK) != -1) {  //F_OK checks if file exists, R_OK checks if file can be read
					//file exists

					char *postrequest;
					//asprintf(&postrequest, "POST /%s HTTP/1.1\n", filename);
					asprintf(&postrequest, "HTTP/1.1 200 OK\n");
					//asprintf(&postrequest, "%sHost: notarealaddress\n", postrequest);
					//asprintf(&postrequest, "%sConnection: keep-alive\n");
					//asprintf(&postrequest, "%sConnection: close\n", postrequest);
					//asprintf(&postrequest, "%sContent-Length: 44\n", postrequest); //TODO set this to actual file size
					//asprintf(&postrequest, "%sCache-Control: no-cache\n", postrequest);
					//asprintf(&postrequest, "%sOrigin: Server program info\n", postrequest);
					//asprintf(&postrequest, "%sUser-Agent: Server machine info\n", postrequest);
					//	asprintf(&postrequest, "%sContent-Type: text/html\n", postrequest);
					//	asprintf(&postrequest, "%s\n<html><body><h1>It works!</h1></body></html>\n", postrequest);


					long fileSize;

					if(strcmp(fileExtension, "html")==0 || strcmp(fileExtension, "htm")==0 || strcmp(fileExtension, "txt")==0 ) {
						char *fileContents = readFile(fileLocation, "r"); //"r" to read the file as text
						asprintf(&postrequest, "%sContent-Length: %d\n", postrequest, strlen(fileContents)); //pls work
						asprintf(&postrequest, "%sContent-Type: text/html\n", postrequest);
						asprintf(&postrequest, "%s\n%s", postrequest, fileContents); //append file contents to postrequest
						free(fileContents); //ALWAYS FREE YOUR MALLOCS WHEN DONE, MKAY?!
						n = write(newsocket,postrequest, strlen(postrequest));
					}
					else {
						char *fileContents = readFile(fileLocation, "rb"); //"rb" to read the file as text
						fileSize = getFileSize(fileLocation);
						asprintf(&postrequest, "%sContent-Length: %ld\n", postrequest, getFileSize(fileLocation)); //pls work
						fprintf(stderr, "FileSize: %ld\n", fileSize);
						fprintf(stderr, "%s", fileContents);
						/* TODO
						 * I think the best way to go about attaching the file will be taking the request,
						 * getting the file length, allocating enough memory for both the header and the file
						 * then using memset or a similar function to set blocks of the allocated memory.
						 * This would avoid string issues likely caused by \0 chars and other unprintable chars,
						 * I think. Maybe not.
						 */
						if(strcmp(fileExtension, "jpg")==0 || strcmp(fileExtension, "jpeg")==0) { //IMAGES
							asprintf(&postrequest, "%sContent-Type: image/jpeg\n\n", postrequest);
						}
						else if(strcmp(fileExtension, "png")==0) { //IMAGES
							asprintf(&postrequest, "%sContent-Type: image/png\n\n", postrequest);
						}
						else if(strcmp(fileExtension, "gif")==0) { //IMAGES
							asprintf(&postrequest, "%sContent-Type: image/gif\n\n", postrequest);
						}
						else if(strcmp(fileExtension, "ico")==0) { //IMAGES
							asprintf(&postrequest, "%sContent-Type: image/icon\n\n", postrequest);
						}

						long headerSize = strlen(postrequest);
						//asprintf(&postrequest, "%s%s", postrequest, fileContents); //append file contents to postrequest
						char *response = malloc(headerSize + fileSize);
						memcpy(response, (void *)postrequest, headerSize); //copy the header into our new memory chunk
						memcpy(response+headerSize, (void *)fileContents, fileSize); //append the file to our memory chunk

						fprintf(stdout, "\n\nRESPONSE:\n%s", response);
						//n = write(newsocket,postrequest, headerSize + fileSize);
						n = write(newsocket, response, headerSize + fileSize);
						free(fileContents); //ALWAYS FREE YOUR MALLOCS WHEN DONE, MKAY?!
						free(response); //free the response too
					}
				}
				else {
					char *postrequest;
					asprintf(&postrequest, "HTTP/1.1 404 Not Found\n");
					//TODO rest of 404 error response
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


char* readFile(char *fileLocation, char *readMode) {

	char *buffer = 0; //initializes buffer to be empty
	long fileSize;
	FILE *fp = fopen(fileLocation, readMode);
	//We already checked with access() if we could read the file, but lets check again for redundancy
	if(fp) {
		fseek(fp, 0, SEEK_END); //sets the file pointer to the end of the file
		fileSize = ftell (fp); //gets the size of the file
		fseek(fp, 0, SEEK_SET); //sets the file pointer to the beginning of the file
		if(strcmp(readMode, "r")==0) { //if we're reading it as text, we'll need an extra char in the array for a \0
			buffer = malloc(fileSize+1); //+1 because \0
			if(buffer) //check malloc success
				fread(buffer, 1, fileSize, fp); //TODO check syscall for errors. Shouldn't be any, but....
		}
		else { //reading as binary
			buffer = malloc(fileSize);
				if(buffer) //check malloc success
					fread(buffer, 1, fileSize, fp); //TODO check syscall for errors. Shouldn't be any, but....
		}
		fclose(fp);
		if(strcmp(readMode, "r")==0)
			buffer[fileSize] = '\0';
		return buffer;
	}
}

long getFileSize(char *fileLocation) {
	long fileSize;
	FILE *fp = fopen(fileLocation, "rb");
	if(fp) {
		fseek(fp, 0, SEEK_END); //sets the file pointer to the end of the file
		fileSize = ftell (fp); //gets the size of the file
		fclose(fp);
		return fileSize;
	}
	fprintf(stderr, "SOMETHING WENT WRONG! File can't be opened at getFileSize(%s)\n", fileLocation);
	return 0; //if can't be opened, return fileSize of 0;
}









