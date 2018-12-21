#include "sock.h"

int sendResponse(int newsockfd, int status, char *buffer) {
	int msgSize;
    	int n;
	buffer[1] = status;
    	msgSize = strlen(buffer + 1);
    	buffer[0] = msgSize;
    	/* Write a response to the client */
    	n = write(newsockfd, buffer, strlen(buffer)); // send on Windows
	
	if (n < 0) {
        	printf("ERROR writing to socket\n");
    	}

	return n;
}
