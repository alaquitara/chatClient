/*
Alexander Laquitara
CS 372 project 1
Description: Design and implement a simple chat system that works for one pair of users, i.e., create two programs: a chat server and a chat client.
1. chatserve starts on host A.
 2. chatserve on host A waits on a port (specified by command-line) for a client request. 
 3. chatclient starts on host B, specifying host A’s hostname and port number on the command line. 
 4. chatclient on host B gets the user’s “handle” by initial query (a one-word name, up to 10 characters).  chatclient will display this handle as a prompt on host B, and will prepend it to all messages sent to host A.  e.g.,  “ SteveO> Hi!!”
 5. chatclient on host B sends an initial message to chatserve on host A : PORTNUM. This causes a connection to be established between Host A and Host B.  Host A and host B are now peers, and may alternate sending and receiving messages.  Responses from host A should have host A’s “handle” prepended. 
 6. Host A responds to Host B, or closes the connection with the command “\quit” 
 7. Host B responds to Host A, or closes the connection with the command “\quit” 
 8. If the connection is not closed, repeat from 6. 
 9. If the connection is closed, chatserve repeats from 2 (until a SIGINT is received).
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>	//This header file contains definitions of a number of data types used in system calls. These types are used in the next two include files
#include<sys/socket.h>	//For socket library
#include<netinet/in.h>	//internet addressing library
#include<netdb.h>
#include <stdbool.h> // for use of booleans


void error(char* msg);
void chatClient(int socket);

#define maxSize 500 //size of message
#define maxMessageSize 510 //message + handleLen
#define	handleLen	11//Length of 10 plus the null terminator

int main(int argc, char *argv[]){
	//check for 2 agrs: hostname and port number
	if(argc < 2){
		error("usage: ./chatclient <server-hostname> <port#>");
	}

	int sockfd, serverPort;
	struct sockaddr_in servAddr; //http://beej.us/guide/bgnet/output/html/multipage/sockaddr_inman.html
	struct hostent* server;
	
	//varify we have a socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket creation"); //https://www.tutorialspoint.com/c_standard_library/c_function_perror.htm
		exit(1);
	}
	
	//Reference http://beej.us/guide/bgnet/output/html/multipage/gethostbynameman.html,
	//Get IP address of server.
	server = gethostbyname(argv[1]);

	//check to make sure gethostbyname returned a valid server.
	if(server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	exit(0);
	}

	//make sure serverAddr is zeroed out, reference: https://www.ibm.com/support/knowledgecenter/SSLTBW_1.13.0/com.ibm.zos.r13.bpxbd00/rbzero.htm
	bzero((char*) &servAddr, sizeof(servAddr));
	
	//sin_family in a sockaddr_in struct should be AF_INET,  I did see some stuff aobut AF_INET6 but left it alone.
	servAddr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*) &servAddr.sin_addr.s_addr, server->h_length);

	//get port number from 2nd argument and change from string to integer.
	serverPort= atoi(argv[2]);
	
	//set port number of socket to portno arg
	//The htons(hostshort) function converts the unsigned short integer hostshort from host byte order to network byte order
	servAddr.sin_port = htons(serverPort);

	//connect the socket
	if(connect(sockfd, (struct sockaddr*) &servAddr, sizeof(servAddr)) == -1){
		perror("socket connection");
		exit(1);
	}
	printf("Connected to the remote host\n");	
	chatClient(sockfd);
	return 0;
}

//fuction called in main to run the chat client, takes a socket int as a parameter
void chatClient(int socket){
	/*"ssize_t data type is used to represent the sizes of blocks that can be read or written in a single operation. It is similar to size_t , but must be a signed type 
	    Reference : http://www.delorie.com/gnu/docs/glibc/libc_239.html*/
	ssize_t n;
	char rcvdMes[maxSize], sentMes[maxSize], finalMes[maxMessageSize], handle[handleLen];
	bool quit = 0, valid = 0;
	int check = 0;
	char* cmdQuit= "\\quit";;
	//Allow user to assign a handle.
	while(valid != 1){		
		printf("Enter a handle you wish to use: ");
		fgets(handle, handleLen, stdin);
		// use gets and manage newline buffer. reference: http://www.cplusplus.com/reference/cstdio/fgets/
		char* pos;
		if((pos = strchr(handle, '\n')) != NULL){
			*pos = '\0';
		}
		check = strlen(handle);
		if(check > handleLen){
			printf("Handle is too long, Max length = %d\n", handleLen);
		}else{
			valid = 1;
		}
	}

	while(quit != 1){
		valid = 0;
		//Reference: http://www.mkssoftware.com/docs/man3/bzero.3.asp
		//The bzero() function copies maxSize bytes, each with a value of zero, into string sentMes.
		bzero(sentMes, maxSize);	
	
		//prepare message
		while(valid != 1){
			printf("%s > Enter a message or '\\quit' to end : ", handle);	
			fgets(sentMes, maxSize, stdin);	
			check = strlen(sentMes);
			if(check > maxSize){
				printf("message size invalid! Max length = %d\n", maxSize);
			}else{
				valid = 1;
			}
		}
		char* pos;
		if((pos = strchr(sentMes, '\n')) != NULL){
			*pos = '\0';
		}
		
		check = strlen(sentMes);
		
		//check to see if user wishes to quit.
		if(strcmp(sentMes, cmdQuit) == 0){
			printf("connection closed by client\n");
			n = write(socket, sentMes, check);
			quit = true;	
			break;
		}	

		check = sprintf(finalMes, "%s > %s\n", handle, sentMes);
		if(check < 0){
			printf("sprintf error of sent final message\n");
		}
		
		//send message to server
		n = write(socket, finalMes, check);
		if(n < 0){
			printf("error: did not write to server\n");
		}
		printf("waiting for server response \n\n");
		//using bzero same as above
		bzero(rcvdMes, maxSize);

		//reference: http://beej.us/guide/bgnet/output/html/multipage/recvman.html
		n = recv(socket, rcvdMes, maxSize, 0);	
		
		if(n < maxSize){
			if(strcmp(rcvdMes, cmdQuit) == 0){
				printf("connection closed by server\n");
				quit = 1;
			}
			else if(n == -1){
				printf("Failed to receive message: -1\n");
			}else if((0 < n) < maxSize){
				printf("chatserve > %s\n\n", rcvdMes);
				
			}else if(n == 0){
				printf("nothing follows");
				quit = 1;
			}
		}
	}
	//following lecture instructions
	close(socket);
	return;
}