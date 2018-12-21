#include "sock.h"

int sendRequest(int sockfd, char *buffer) {
	int msgSize = strlen(buffer + 1);
	int n;
	buffer[0] = msgSize;
	/* Send message to the server */
	n = write(sockfd, buffer, strlen(buffer));
	if (n < 0) {
		printf("ERROR writing to socket\n");
		close(sockfd);
		return -1;
	}
	return 0;
}

