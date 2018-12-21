#include "sock.h"

void closeSocket(int sockfd) {
    shutdown(sockfd, 2);
    close(sockfd);
}
