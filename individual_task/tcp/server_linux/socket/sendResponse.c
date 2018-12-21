#include "sock.h"

int sendResponse(int newsockfd, int status, char *buffer) {
	int msgSize;
    	int n;
	buffer[1] = status;
    	msgSize = strlen(buffer + 1);
    	buffer[0] = msgSize;
    	/* Write a response to the client */
    	n = write(newsockfd, buffer, strlen(buffer)); // send on Windows
	return n;
}
