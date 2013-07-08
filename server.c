#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT "9060"

/**
	http://www.linuxquestions.org/questions/programming-9/client-server-chat-program-4175415420/

	Build: gcc -o server server.c

	server.c
	Version 2013-07-03

	Copyright (c) 2012 amboxer21 at LinuxQuestions.org
	Copyright (c) 2013 DJC Gonzales

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.

	amboxer21 at LinuxQuestions.org

	This source has been modified to encrypt the communication between the client and the server using an algorithm that I designed.

	DJC Gonzales, di.gonzales@gmail.com

*/
int main(int argc, char *argv[]) {

	int serv_sock, client_sock, port, binder, listener;
	ssize_t bytes_read, bytes_written;
	struct sockaddr_in server_addr, client_addr;
	struct hostent *server;
	int yes = 1;
	socklen_t client_sock_len;
	char data[4096];

	/** argument count validation */
	if(2 != argc) {
		fprintf(stderr, "Usage: %s + <serverHost>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/** create socket with error checking */
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if(serv_sock == 0) {
		printf("SOCKET(0) error ---> %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if(serv_sock < 0) {
		printf("SOCKET(-1) error ---> %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	} else if(serv_sock) {
		do {
			{
			printf("Waiting for a connection...\n");
			}
		} while(!accept);
	}

	/** enable address reuse (SO_REUSEADDR) */
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	/** set serv_addr to 0, get server address, with error checking */
	bzero(&server_addr, sizeof(server_addr));
	server = gethostbyname(argv[1]);

	if(server == NULL) {
		fprintf(stderr, "No such host.\n");
		printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/** bind to own port, with error checking */
	port = atoi(PORT);
	server_addr.sin_family = AF_INET;
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	server_addr.sin_port = htons(port);

	binder = bind(serv_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));

	if(binder < 0) {
		printf("BIND(-1) error ---> %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/** establish socket listener for client, with error checking */
	listener = listen(serv_sock, 20);

	if(listener < 0) {
		printf("LISTEN(-1) error ---> %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/** accept client socket, with error checking */
	client_sock_len = sizeof(client_addr);

	client_sock = accept(serv_sock, (struct sockaddr *) &client_addr, &client_sock_len);

	if(client_sock == 0) {
		printf("ACCEPT(0) error ---> %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(client_sock < 0) {
		printf("ACCEPT(-1) error ---> %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(accept) {
	  {
	  printf("Received connection from %s.\n", inet_ntoa(client_addr.sin_addr));
	  }
	}

	/** performs this while there's still no error */
	for( ; ; ) {
		/** Receive message from client, with error checking */
		// TODO insert decompression here
		// TODO insert decryption here
		bytes_read = read(client_sock, data, sizeof(data));

		if(bytes_read < 0) {
			printf("READ(-1) error ---> %s.\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if(bytes_read == 0) {
			printf("Client has closed the connection.");
			exit(EXIT_SUCCESS);
		}

		fprintf(stdout, "Client: %s", data);

		/** Send message to client, with error checking */
		printf("Server: ");
		fgets(data, sizeof(data), stdin);
		// TODO insert encryption here
		// TODO insert compression here	
		bytes_written = write(client_sock, data, strlen(data));

		if(bytes_written < 0) {
			printf("WRITE(-1) error ---> %s.\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/** close client, own socket */
	close(client_sock);
	close(serv_sock);

	return 0;
}
