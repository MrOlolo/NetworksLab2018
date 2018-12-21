#include "socket/sock.h"
#include "product/product.h"

#define THREAD_ERR "Can't create thread, status = %d\n"
#define SERVER_OFF "SERVER OFFLINE.\nBYE\n"

int usersCount = 0;
int userSocks[MAX_USERS];

void readFileToBuffer(char* buffer, const char* filename) {
	FILE *file;
	bzero(buffer, BUF_LEN);
	if ((file = fopen(filename,"r")) == NULL) {
		perror("ERROR occured while opening file");
		//return -1;
	} else {
		char ch;
		int i = 0;
		while((ch = getc(file)) != EOF) {
			buffer[++i] = ch;
		}
	}
	fclose(file);
}

void deleteSockAndSort(int sock) {
	int sort = 0;
	for (int i = 0; i < usersCount - 1; i++) {
		if (userSocks[i] == sock) {
			sort = 1;
		}
		if (sort) {
			userSocks[i] = userSocks[i + 1];
		}	
	}
}

void deleteUserAndSort(int id) {
	for (int i = id; i < usersCount - 1; i++) {
		userSocks[i] = userSocks[i + 1];
	}
}

void endThread(int sockfd) {
    printf("Close client thread, socket %d\n", sockfd);
    closeSocket(sockfd);
	usersCount--;
	deleteSockAndSort(sockfd);
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

int processRequest(char *buffer) {
	char tmp[NAME_LENGTH];
	bzero(tmp, NAME_LENGTH);
	enum edit {add, buy } action;	 
	
	processRequestCycle(buffer, tmp);
	if (!strcmp(tmp, "show")) {
		bzero(buffer, BUF_LEN);
		productsToBuffer(buffer + 2);
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

void *communicateWithNewClient(void *args) {
    /* If connection is established then start communicating */
    ssize_t n;
    int newsockfd = *(int*) args;
	char buf[BUF_LEN];
	int status;
	
	while(1) {
		bzero(buf, BUF_LEN);
		n = getRequest(newsockfd, buf);
		if (n < 0) {
			printf("Problem with socket\n");
			break;
		}
		status = processRequest(buf);
		sendResponse(newsockfd, status, buf);
		if (n < 0) {
			printf("Problem with socket\n");
			break;
		}
	} 
    endThread(newsockfd);
    return 0;
}

void *listenForConnection() {
    int newsockfd, status;
    unsigned int clilen;
    struct sockaddr_in cli_addr;

    if (listen(sockfd, 5) < 0) {
        perror( "listen error!\n");
        return 0;
    }

    clilen = sizeof(cli_addr);

    pthread_t communicationThread;

    while (serverWorking) {
		if (usersCount == MAX_USERS){
			continue;
		}
        /* Accept actual connection from the client */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (!serverWorking) {
            break;
        }

        if (newsockfd < 0) {
            perror("ERROR on accept");
            break;
        }

        status = pthread_create(&communicationThread, NULL, communicateWithNewClient,&newsockfd);
        if (status != 0) {
            printf(THREAD_ERR, status);
        } else {
			userSocks[usersCount] = newsockfd;
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
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    pthread_t listeningThread;
    status = pthread_create(&listeningThread, NULL, listenForConnection, NULL);
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
			strcat(buffer + 2, "All users disconnected");
			for (int i = 0; i < usersCount; i++) {
				sendResponse(userSocks[i], DISCONNECT,  buffer);
				closeSocket(userSocks[i]);
				deleteUserAndSort(i);
			}
			
		} else if (!strcmp(comm_buf, "2\n")) {
			
			printf("WRITE USER ID\n");
			bzero(comm_buf, BUF_LEN);
			fgets(comm_buf, BUF_LEN - 1, stdin);
			int id = atoi(comm_buf);
			printf("id = %d\n", id);
			bzero(buffer, BUF_LEN);
			strcat(buffer + 2, "You disconnected");
			if (id >= 0 && id < usersCount) {
				sendResponse(userSocks[id], DISCONNECT,  buffer);
				closeSocket(userSocks[id]);
				deleteUserAndSort(id);
			} else {
				printf("Wrong id\n");
			}
			
		} else if (!strcmp(comm_buf, "3\n")) {
			
			printf("USERS LIST:\n");
			
			if (usersCount == 0) {
				printf("Server empty\n");
			} else {
				for (int i = 0; i < usersCount; i++) {
					printf("%d. socket: %d\n", i , userSocks[i]);
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
