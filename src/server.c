#define _GNU_SOURCE			//Need this for asprintf(), otherwise we get implicit declaration
#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <strings.h>        //Strings Library
#include <string.h>         //String Library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <unistd.h>         //Needed for access() to check file information, as well as read() and write()

/**
 * Listen for the client, send request, and make available its services
 *
 * @author Jeffrey Morton
 * @author Thanh Tran
 * @date 03/19/2018
 * @info Course COP4635
 */

#define BUFFER_MAX_SIZE 2048 //Defining a constant for max buffer size. Cleaner than magic numbers


char* readFile(char *fileLocation, char *readMode);
long getFileSize(char *fileLocation);
void return404(int newsocket); 
void return500(int newsocket); 

//The server
int main(int argc, char *argv[]){

	struct sockaddr_in server_address, client_address;
	int socket_hold, newsocket, port, n;
	char buffer[BUFFER_MAX_SIZE];
	size_t client;

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

	fprintf(stderr,"\nServer commence\n\nReadying...\n\nWaiting and listening for client request...\n\n");

	//MULTITHREADING
	int pid; 
	listen(socket_hold,5);//(reference to file descriptor, max queue)
	client = sizeof(client_address);
	while(1==1) { //while true (runs forever)

		newsocket = accept(socket_hold,(struct sockaddr *) &client_address, (socklen_t *) &client);//(file descriptor socketed-binded-listened,pointer to sockaddr struct, sizeof pointed struct)

		if (newsocket < 0){
			fprintf(stderr,"Accept failed");
			exit(1);
		}

		pid = fork(); //creating child process
		if (pid < 0) { //something went wrong, pid is only <0 when a fork error occurs
			fprintf(stderr,"ERROR on fork");
			exit(1);
		}

		if (pid == 0) { //THIS IS NOW THE CHILD PROCESS! DO STUFFS!
			close(socket_hold); //freeing this up, no longer needed
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
				if(token==NULL) //error checking
					return404(newsocket);
				token = strtok(NULL, delim);  // every call with NULL uses saved user_input value and returns next substring
				if(token==NULL) //error checking
					return404(newsocket);
				token++;
				char filename[strlen(token)+1]; //+1 for \0
				strcpy( filename, token); //filename now saved
				strcpy(delim,"."); //going for the file extension now
				strcpy(temp, filename);
				token = strtok(temp, delim); //this part is file name without extension
				if(token==NULL) //error checking
					return404(newsocket);
				token = strtok(NULL, delim); //YEEEEES, THIS IS THE GOOD STUFF!
				if(token==NULL) //error checking
					return404(newsocket);
				char fileExtension[strlen(token)+1]; //+1 for \0
				strcpy(fileExtension, token); //ALL YOUR TOKENS ARE BELONG TO MEEEE!


				if(strlen(filename)>0 && strlen(fileExtension)>0) {
					char *fileLocation;
					asprintf(&fileLocation, "./%s", filename);
					if(access(fileLocation, R_OK) != -1) {  //F_OK checks if file exists, R_OK checks if file can be read
						//file exists

						char *postrequest;
						asprintf(&postrequest, "HTTP/1.1 200 OK\n");
						//THESE ARE NOT NEEDED FOR RESPONSES, BUT THEY ARE HERE FOR REFERENCE
						//asprintf(&postrequest, "%sHost: notarealaddress\n", postrequest);
						//asprintf(&postrequest, "%sConnection: keep-alive\n");
						//asprintf(&postrequest, "%sConnection: close\n", postrequest);
						//asprintf(&postrequest, "%sCache-Control: no-cache\n", postrequest);
						//asprintf(&postrequest, "%sOrigin: Server program info\n", postrequest);
						//asprintf(&postrequest, "%sUser-Agent: Server machine info\n", postrequest);


						long fileSize;

						if(strcmp(fileExtension, "html")==0 || strcmp(fileExtension, "htm")==0 || strcmp(fileExtension, "txt")==0 ) {
							char *fileContents = readFile(fileLocation, "r"); //"r" to read the file as text
							if(fileContents==NULL)
								return500(newsocket);
							asprintf(&postrequest, "%sContent-Length: %zu\n", postrequest, strlen(fileContents)); //pls work
							asprintf(&postrequest, "%sContent-Type: text/html\n", postrequest);
							asprintf(&postrequest, "%s\n%s", postrequest, fileContents); //append file contents to postrequest
							free(fileContents); //ALWAYS FREE YOUR MALLOCS WHEN DONE, MKAY?!
							n = write(newsocket,postrequest, strlen(postrequest));
							fprintf(stdout,"%s\n\n\n",postrequest);
						}
						else {
							char *fileContents = readFile(fileLocation, "rb"); //"rb" to read the file as text
							if(fileContents==NULL)
								return500(newsocket);
							fileSize = getFileSize(fileLocation);
							asprintf(&postrequest, "%sContent-Length: %zu\n", postrequest, getFileSize(fileLocation));

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
							char *response = malloc(headerSize + fileSize);
							memcpy(response, (void *)postrequest, headerSize); //copy the header into our new memory chunk
							memcpy(response+headerSize, (void *)fileContents, fileSize); //append the file to our memory chunk

							n = write(newsocket, response, headerSize + fileSize);
							fprintf(stdout,"%s\n\n\n",response);
							free(fileContents); //ALWAYS FREE YOUR MALLOCS WHEN DONE, MKAY?!
							free(response); //free the response too
						}
					}
					else {
						return404(newsocket);
					}
				}
				else {
					return404(newsocket);
				}
				if (n < 0){
					fprintf(stderr,"Writing to socket fail");
					exit(1);
				}
			}
			exit(0); //closing this thread
		}
		else {
			close(newsocket); //closing the socket
		}
	}
	return 0;
}
/**
 * Open the file
 *
 *@param fileLocation the file path
 *@param readMode the file mode of operation
 *
 *@return  the file that was opened
 */
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
	else
		return NULL;
}

/**
 * Measure the file length
 *
 *@param fileLocation the file path
 *
 *@return  file size
 */
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

/**
 * Invalid 400 page function. Responds with a 500 internal server error header
 * and corresponding 500.html page
 *
 *@param newsocket socket current status
 */
void return404(int newsocket) {
	char *postrequest;
	asprintf(&postrequest, "HTTP/1.1 404 Not Found\n");
	char *fileContents = readFile("./404.html", "r"); //"r" to read the file as text
	asprintf(&postrequest, "%sContent-Length: %zu\n", postrequest, strlen(fileContents)); //pls work
	asprintf(&postrequest, "%sContent-Type: text/html\n", postrequest);
	asprintf(&postrequest, "%s\n%s", postrequest, fileContents); //append file contents to postrequest
	free(fileContents); //ALWAYS FREE YOUR MALLOCS WHEN DONE, MKAY?!
	int n = write(newsocket,postrequest, strlen(postrequest));
	fprintf(stdout,"%s\n\n\n",postrequest);
	if (n < 0){
		fprintf(stderr,"Writing to socket fail");
		exit(1);
	}
	exit(0);
}

/**
 * Invalid 500 page function. Responds with a 500 internal server error header
 * and corresponding 500.html page
 *
 *@param newsocket socket current status
 */
void return500(int newsocket) {
	char *postrequest;
	asprintf(&postrequest, "HTTP/1.1 500 Internal Server Error\n");
	char *fileContents = readFile("./500.html", "r"); //"r" to read the file as text
	asprintf(&postrequest, "%sContent-Length: %zu\n", postrequest, strlen(fileContents)); //pls work
	asprintf(&postrequest, "%sContent-Type: text/html\n", postrequest);
	asprintf(&postrequest, "%s\n%s", postrequest, fileContents); //append file contents to postrequest
	free(fileContents); //ALWAYS FREE YOUR MALLOCS WHEN DONE, MKAY?!
	int n = write(newsocket,postrequest, strlen(postrequest));
	fprintf(stdout,"%s\n\n\n",postrequest);
	if (n < 0){
		fprintf(stderr,"Writing to socket fail");
		exit(1);
	}
	exit(0);
}



