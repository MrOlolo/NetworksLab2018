#include "sock.h"

int clientCreateSock(char *host, char *port)  {
	int sockfd;
    	uint16_t portno;
    	struct sockaddr_in serv_addr;
    	struct hostent *server;
	portno = (uint16_t) atoi(port);
 	/* Create a socket point */
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) {
        	perror("ERROR opening socket");
        	return -1;
    	}

    	server = gethostbyname(host);
    	if (server == NULL) {
        	fprintf(stderr, "ERROR, no such host\n");
        	close(sockfd);
        	return -1;
    	}

    	bzero((char *) &serv_addr, sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;
    	bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    	serv_addr.sin_port = htons(portno);
    	/* Now connect to the server */
    	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        	perror("ERROR connecting");
        	close(sockfd);
        	return -1;
    	}
	return sockfd;
}
