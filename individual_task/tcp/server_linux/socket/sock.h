#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define BUF_LEN 256
#define LIST_SIZE 30
#define PORT 5001
#define MAX_USERS 10
#define NAME_LENGTH 30

#define DISCONNECT 66
#define BAD_REQUEST 40
#define SERVER_NOT_FOUND 53
#define OK 20

int sockfd;
int serverWorking;

void getClearMsg(char* buf);

int getRequest(int newsockfd, char* buffer);

int sendResponse(int newsockfd, int status, char *buffer);

void closeSocket(int sockfd);