#include "sock.h"

void closeSocket(int sockfd) {
    printf(CLOSE_SOCK, sockfd);
    shutdown(sockfd, 2);
    close(sockfd);
}
