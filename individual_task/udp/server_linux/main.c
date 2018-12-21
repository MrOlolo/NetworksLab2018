#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include "product/product.h"


#define COMMUNICATING "Communication with client starts\n"
#define CLOSE_THR "Close thread\n"
#define THREAD_ERR "Can't create thread, status = %d\n"
#define SERVER_OFF "SERVER OFFLINE.\nBYE\n"

#define BUF_LEN 256
#define LIST_SIZE 30
#define PORT 5001
#define MAX_USERS 10
#define NAME_LENGTH 30

#define DISCONNECT 66
#define BAD_REQUEST 40
#define PREVIOUS_MSG_LOST 53
#define BAD_USER 38
#define USER_ID 33
#define OK 20

#define CLOSE_SOCK "Close socket %d\n"

int sockfd;
int serverWorking;

struct user_info {
	bool connected;
	int msgNumber;
};
struct user_info usersinfo[MAX_USERS];
int usersCount = 0;


void closeSocket(int sockfd) {
    printf(CLOSE_SOCK, sockfd);
    shutdown(sockfd, 2);
    close(sockfd);
}

void getClearMsg(char* buf) {
	strcpy(buf, buf + 2);
}

void readFileToBuffer(char* buffer, const char* filename) {
	FILE *file;
	bzero(buffer, BUF_LEN);
	if ((file = fopen(filename,"r")) == NULL) {
		perror("ERROR occured while opening file");
		
	} else {
		char ch;
		int i = 0;
		while((ch = getc(file)) != EOF) {
			buffer[++i] = ch;
		}
	}
	fclose(file);
}

void deleteUser(int id) {
	if(usersinfo[id - 1].connected) {
		usersCount--;
		usersinfo[id - 1].connected = false;
	}
}

int getUserID() {
	for (int i = 0; i  < MAX_USERS; i++) {
		if (!usersinfo[i].connected) {
			return i + 1;
		} 
	}
	return 0;
}

void endThread() {
    printf(CLOSE_THR);
    closeSocket(sockfd);
    pthread_exit(0);
}

void processRequestCycle(char* buffer, char* tmp) {
	int len = strlen(buffer);
	int i = 0;
	char ch;
	while (((ch = buffer[i]) != '\n') && (i < len) && (i < NAME_LENGTH)) {	
		tmp[i] = ch;
		i++;
	}
	strcpy(buffer, buffer + i + 1);
}

bool isNewUser(char *buffer) {
	if(buffer[0] == 0 && buffer[1] == 0) {
		return true;
	} else {
		return false;
	}
}

int processRequest(char *buffer) {
	char tmp[NAME_LENGTH];
	bzero(tmp, NAME_LENGTH);
	enum edit {add, buy } action;	 
	
	processRequestCycle(buffer, tmp);
	if (!strcmp(tmp, "show")) {
		bzero(buffer, BUF_LEN);
		productsToBuffer(buffer + 3);
		return OK;
				
	} else if (!strcmp(tmp, "quit")) {
		bzero(buffer, BUF_LEN);
		return  DISCONNECT;
		
	} else if ( !strcmp(tmp, "buy")) {
		action = buy;
		
	} else if (!strcmp(tmp, "add")) {
		action = add;
		
	} else {
		bzero(buffer, BUF_LEN);
		return BAD_REQUEST;
	}
	
	bzero(tmp, NAME_LENGTH);
	processRequestCycle(buffer, tmp);
	char name[NAME_LENGTH];
	bzero(name, sizeof(name));
	strcat(name, tmp);
	
	bzero(tmp, NAME_LENGTH);
	processRequestCycle(buffer, tmp);
	int amount = atoi(tmp);
	
	bzero(buffer, BUF_LEN);
	switch(action) {
		case add:
			addProduct(name, amount);
			return OK;
		case buy:
			if (buyProduct(name, amount) != 0) {
				return BAD_REQUEST;
			}
			return OK;
		default:
			return BAD_REQUEST;
	}
}

void *communicateWithClients() {
    int userID, status;
    unsigned int clilen;
    struct sockaddr_in cli_addr;
	char* packet_addr;
	ssize_t n;
	char msg_buffer[BUF_LEN];
	bool isNew;
	bool isLeft ;

    clilen = sizeof(cli_addr);

    while (serverWorking) {
		//printf("Current users count %d\n", usersCount);
        /* Accept actual connection from the client */
		isLeft = false;
		isNew = false;
        bzero(msg_buffer, BUF_LEN);
		n = recvfrom(sockfd, msg_buffer, sizeof(msg_buffer), 0, (struct sockaddr *)&cli_addr, &clilen);

        if (!serverWorking) {
            break;
        }

        if (n< 0) {
            perror("ERROR on accept");
            continue;
        }
		
		packet_addr = inet_ntoa(cli_addr.sin_addr);
		if (packet_addr == NULL) {
			perror("ERROR on inet_ntoa");
		}
		//printf("Message from: %s\n", packet_addr);
		//printf("Here is the message: %s\n", msg_buffer + 2);
		userID = msg_buffer[0] - 1 ;
		
		if (isNewUser(msg_buffer)) {
			if (usersCount == MAX_USERS){
				continue;
			}
			bzero(msg_buffer, BUF_LEN);
			msg_buffer[0] = USER_ID;
			userID = getUserID();
			msg_buffer[1] = userID;
			msg_buffer[2] =1;
			
			isNew = true;
		} else if ((userID ) > MAX_USERS || !usersinfo[userID].connected) {
				bzero(msg_buffer, BUF_LEN);
				msg_buffer[0] = BAD_USER;
				
		} else if (usersinfo[userID].msgNumber != msg_buffer[1]) {
				bzero(msg_buffer, BUF_LEN);
				msg_buffer[0] = PREVIOUS_MSG_LOST;
				msg_buffer[1] = userID + 1;
				msg_buffer[2] = usersinfo[userID].msgNumber;
				
		} else {
				getClearMsg(msg_buffer);
				status = processRequest(msg_buffer);
				
				if (status  == DISCONNECT) {
					isLeft = true;
				}
				msg_buffer[0] = status;
				msg_buffer[1] = userID + 1;
				usersinfo[userID].msgNumber++;
				msg_buffer[2] = usersinfo[userID].msgNumber;
		}
		
        n = sendto(sockfd, msg_buffer, strlen(msg_buffer), 0, (const struct sockaddr *)&cli_addr, clilen);

		if (isLeft) {
			usersinfo[userID].connected = false;
			usersCount--;
			printf("USER DISCONNECT\n");
			printf("Current users count %d\n", usersCount);
		}
		
		if (n < 0) {
			perror("ERROR writing to socker");
			
		}  else  if (isNew) {
			usersinfo[userID - 1].connected = true;
			usersinfo[userID - 1].msgNumber = 1;
			usersCount++;
			printf("NEW USER CONNECT\n");
			printf("Current users count %d\n", usersCount);
		}
    }
	pthread_exit(0);
    return 0;
}

int main() {
    int status;
    uint16_t portno;
    struct sockaddr_in serv_addr;
	prod_list_size = 0;
	readProductsList();
    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        closeSocket(sockfd);
        exit(1);
    }
	
	serverWorking = 1;
    pthread_t communicationThread;
    status = pthread_create(&communicationThread, NULL, communicateWithClients, NULL);
    if (status != 0) {
        printf(THREAD_ERR, status);
        closeSocket(sockfd);
        exit(1);
    }

	char serv_comm[BUF_LEN];
	char comm_buf[12];
	readFileToBuffer(serv_comm, SERVER_COMMANDS);
    while (1) {
		printf("%s", serv_comm + 1);
		bzero(comm_buf, 12);
		fgets(comm_buf, 11, stdin);
		char buffer[BUF_LEN];
        if (!strcmp(comm_buf, "4\n")) {
            serverWorking = 0;
            break;
			
        } else if (!strcmp(comm_buf, "1\n")) {
			bzero(buffer, BUF_LEN);
			for (int i = 1; i <= MAX_USERS ; i++) {
				deleteUser(i);
				printf("User with id = %d  deleted.\n", i);
			}
			
		} else if (!strcmp(comm_buf, "2\n")) {
			
			printf("WRITE USER ID\n");
			bzero(comm_buf, BUF_LEN);
			fgets(comm_buf, BUF_LEN - 1, stdin);
			int id = atoi(comm_buf);
			printf("id = %d\n", id);
			bzero(buffer, BUF_LEN);
			if (id > 0 && usersinfo[id - 1].connected) {
				deleteUser(id);
				printf("User with id = %d  deleted.\n", id);
			} else {
				printf("Wrong id\n");
			}
			
		} else if (!strcmp(comm_buf, "3\n")) {
			
			printf("USERS LIST:\n");
			
			if (usersCount == 0) {
				printf("Server empty\n");
			} else {
				printf("Current users count %d\n", usersCount);
				for (int i = 0; i < MAX_USERS; i++) {
					printf("UserID = %d,  connected: %d\n", i + 1 , usersinfo[i].connected);
				}
			}
			printf("\n");
			
		} else  if (!strcmp(comm_buf, "clear\n")) {
			system("clear");
		}
    }
    closeSocket(sockfd);
    printf(SERVER_OFF);
	writeProductsToFile();
    sleep(1);
    return 0;
}
