#include "sock.h"

int getResponse(int sockfd, char *buffer) {
	int n;
	int realMsgLen;
	int curMsgLen;
	/* Now read server response */
	bzero(buffer, MSG_BUF_LEN);
	n = read(sockfd, buffer, MSG_BUF_LEN - 1);
	if (n <= 0) {
		printf("ERROR reading from socket\n");
		close(sockfd);
		return -1;
	}
	realMsgLen = (buffer[0]) + 1;
	curMsgLen = n;
	//printf("Message size = %d, current size = %d \n", realMsgLen, curMsgLen);
	while(realMsgLen > curMsgLen) {
	n = read(sockfd, buffer + curMsgLen, realMsgLen - curMsgLen); // recv on Windows
		if (n <= 0) {
			printf("ERROR reading from socket\n");
			close(sockfd);
			return -1;
		}
		curMsgLen += n;
		//printf("Message size = %d, current size = %d \n", realMsgLen, curMsgLen);
	}
	//printf("Status: %d\nMessage: %s\n", buffer [1],  buffer + 2);
	return 0;
}

