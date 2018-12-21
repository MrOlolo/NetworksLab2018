#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

#define MSG_BUF_LEN 256
#define INPUT_BUF_LEN 31

//MESSAGE CODES
#define DISCONNECT 66
#define BAD_REQUEST 40
#define SERVER_NOT_FOUND 53
#define OK 20

int clientCreateSock(char *host, char *port);

int getResponse(int sockfd, char *buffer);

int sendRequest(int sockfd, char *buffer);

void checkResponse(char *buffer, char *msg);
