#include "sock.h"

int getRequest(int newsockfd, char* buffer) {
    ssize_t realMsgLen;
    ssize_t curMsgLen;
    ssize_t n;
	bzero(buffer, sizeof(buffer));
	n = read(newsockfd, buffer, sizeof(buffer) - 1); // recv on Windows
    if (n <= 0) {
		return -1;
    }

    realMsgLen = buffer[0] + 1;
    curMsgLen = n;
    //printf("Message size = %zu, current size = %zu \n", realMsgLen, curMsgLen);
    while(realMsgLen > curMsgLen) {
    	if (!serverWorking) {
			printf("Server off\n");
			return -1;
        }

        n = read(newsockfd, buffer + curMsgLen, realMsgLen - curMsgLen); // recv on Windows
        if (n <= 0) {
			return -1;
        }

        curMsgLen += n;
       // printf("Message size = %zu, current size = %zu \n", realMsgLen, curMsgLen);
    }
	getClearMsg(buffer);
	return 0;
}
