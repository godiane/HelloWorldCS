#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT "9060"

/**
	http://www.linuxquestions.org/questions/programming-9/client-server-chat-program-4175415420/

	Build: gcc -o client.exe client.c

	client.c
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

	int client_sock, remote_port, connector, yes = 1;
	ssize_t bytes_read, bytes_written;
	struct hostent *server;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[4096];

	/** argument count validation */
	if(2 != argc)
	{
		fprintf(stderr, "Usage: %s + <serverHost>\n", argv[0]);
		exit(0);
	}

	/** create socket with error checking */
	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if(client_sock <= 0) {
		printf("SOCKET(-1) error ---> %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(client_sock == 0) {
		printf("SOCKET(0) error ---> %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/** enable address reuse (SO_REUSEADDR) */
	setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	/** set serv_addr to 0, get server address, with error checking */
	bzero(&serv_addr, sizeof(serv_addr));
	server = gethostbyname(argv[1]);

	if(server == NULL) {
		fprintf(stderr, "No such host.\n");
		printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/** connect to server port, with error checking */
	remote_port = atoi(PORT);
	serv_addr.sin_family = AF_INET;
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	serv_addr.sin_port = htons(remote_port);

	connector = connect(client_sock, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));

	if(connector < 0) {
		fprintf(stderr, "%s. CONNECT()\n", strerror(errno));
		exit(EXIT_FAILURE);
	} else {
	    printf("Made a connection to %s\n", inet_ntoa(serv_addr.sin_addr));
	}

	/** performs this while there's still no error */
	for( ; ; )
	{
		/** Send message to server, with error checking */
		printf("Client: ");
		fgets(buffer, sizeof(buffer), stdin);
		// TODO insert encryption here
		// TODO implement compression here
		bytes_written = write(client_sock, buffer, sizeof(buffer));

		if(bytes_written == 0) {
			printf("WRITE(0) error ---> %s.\n", strerror(errno));
			printf("Nothing was written.\n");
			break;
		}

		/** reset buffer */
		memset(buffer, 0, sizeof(buffer));
		// TODO implement decompression here
		// TODO insert decryption here
		/** Receive message from server, with error checking */
		bytes_read = read(client_sock, buffer, sizeof(buffer));

		if(bytes_read < 0) {
			printf("READ(-1) error ---> %s.\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

        if(bytes_read == 0) {
			printf("Server has closed the connection.");
			exit(EXIT_SUCCESS);
        }

		fprintf(stdout, "Server: %s", buffer);
	}

	/** close own socket */
	close(client_sock);
	return 0;
}
