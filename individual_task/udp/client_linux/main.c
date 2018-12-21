#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MSG_BUF_LEN 256
#define INPUT_BUF_LEN 31

#define DISCONNECT 66
#define BAD_REQUEST 40
#define PREVIOUS_MSG_LOST 53
#define USER_ID 33
#define OK 20
#define BAD_USER 38


enum  action {
	add,
	buy
};

int user_id;
int msg_number;
char msg_buffer[MSG_BUF_LEN];
char input_buffer[INPUT_BUF_LEN];
int sockfd;
struct sockaddr_in serv_addr;
unsigned int len = sizeof(serv_addr);

void closeApp(char* error) {
	int code = 0;
	if (strcmp(error ,"")) {
		printf("%s\n", error);
		code = 1;
	}
	printf("Shutdown...\n");
	close(sockfd);
	exit(code);
}

void readFromKeyboard(char *msg) {
	printf("%s\n", msg);
	bzero(input_buffer, INPUT_BUF_LEN);
	fgets(input_buffer , INPUT_BUF_LEN - 1, stdin);
}

int getResponse(int sockfd, char *buffer) {
	int n;
	bzero(buffer, MSG_BUF_LEN);
	//printf("size of buf %d\n", sizeof(buffer));
	n = recvfrom(sockfd, buffer, MSG_BUF_LEN, 0,  (struct sockaddr *)&serv_addr, &len);
	if (n < 0) {
		printf("ERROR reading from socket\n");
		close(sockfd);
		return -1;
	}
	
	//printf("%s\n", buffer);
	//////////////
	////Need to check msg code
	///////////////
	//if (buffer[1] != user_id)
	return 0;
}

int sendRequest(int sockfd, char *buffer) {
	int n;
	buffer[0] = user_id;
	buffer[1] =  msg_number;
	/* Send message to the server */
	n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, len);
	if (n < 0) {
		printf("ERROR writing to socket\n");
		close(sockfd);
		return -1;
	}
	return 0;
}

void checkResponse(char *buffer, char *msg) {
	switch(buffer[0]) {
		case USER_ID:
			user_id = buffer[1];
			msg_number = buffer[2];
			printf("USER_ID = %d, message = %d \n", user_id, msg_number);
			break;
		case OK:
			msg_number = buffer[2];
			printf("%s\n", msg);
			printf("%s\n", buffer + 3);
			break;
		case BAD_REQUEST: 
			printf("BAD REQUEST\n");
			break;
		case PREVIOUS_MSG_LOST:
			printf("PREVIOUS MESSAGE LOST");
			msg_number = buffer[2];
			break;
		case DISCONNECT:
			printf("DISCONNECTED\n");
			printf("%s\n", buffer + 3);
			break;
		case BAD_USER:
			printf("SERVER DON'T KNOW YOU");
			break;
		default:
			printf("UNKNOWN ERROR\n" );
			break;
	}
}

void showProducts() {
	bzero(msg_buffer,  MSG_BUF_LEN);
	strcat(msg_buffer  + 2,  "show\n");
	if  (sendRequest(sockfd,  msg_buffer) == -1)  {
		closeApp("Problem  occurred when try to watch products list");
	}
	if (getResponse(sockfd, msg_buffer) == -1) {
		closeApp("Problem  occurred when try to watch products list");
	}
	checkResponse(msg_buffer, "PRODUCT LIST") ;
}

void workWithProducts(enum action make ) {
	char *com;
	char *success;
	switch (make) {
		case add:
			com = "add\n%s";
			success = "PRODUCT ADDED";
			break;
		case buy:
			com = "buy\n%s";
			success = "PRODUCT BOUGHT";
			break;
		default:
			printf("WRONG ACTION\n");
			return;
	}
	bzero(msg_buffer,  MSG_BUF_LEN);
	readFromKeyboard("Write product name( < 30 char): ");
	if (strlen(input_buffer) == 1) {
		printf("EMPTY NAME\n");
		getchar();
		return;
	}
	sprintf(msg_buffer + 2, com, input_buffer);
	//printf("%s\n", input_buffer);
	readFromKeyboard("Write product amount:");
	char ch;
	int i = 0;
	while ((ch = input_buffer[i]) != '\n' ) {
		if (ch < '0' || ch > '9') {
			printf("BAD INPUT DATA\n");
			getchar();
			return;
		}
		i++;
	}
	
	strcat(msg_buffer + 2, input_buffer);
	//printf("Takaya ditch: %s", msg_buffer + 1);
	
	if  (sendRequest(sockfd,  msg_buffer) == -1) {
		closeApp("Problem occurred when you try to work with product list");
	}
	if  (getResponse(sockfd, msg_buffer) == -1) {
		closeApp("Problem occurred when you try to work with product list");
	}
	checkResponse(msg_buffer,  success) ;
}

void addProduct() {
	workWithProducts(add);
}

void buyProduct() {
	workWithProducts(buy);
}

void quit() {
	printf("Bye\n");
	bzero(msg_buffer, MSG_BUF_LEN);
	strcat(msg_buffer + 2, "quit\n");
	sendRequest(sockfd, msg_buffer);
	getResponse(sockfd,  msg_buffer);
}

int main(int argc, char *argv[]) {
	struct hostent *server;
	uint16_t portno;
	user_id = 0;
	msg_number = 0;

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
	
	portno = (uint16_t) atoi(argv[2]);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		closeApp("ERROR opening socket");
	}
	
	server = gethostbyname(argv[1]);
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	bcopy(server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t)server->h_length);
	
	///Try to get user_id from server
	if  (sendRequest(sockfd,  msg_buffer) == -1) {
		closeApp("Can't get user id\n");
	}
	if  (getResponse(sockfd, msg_buffer) == -1) {
		closeApp("Can't get user id\n");
	}
	checkResponse(msg_buffer,  "") ;
	
	
	printf(">>>WELCOME TO THE KSPT ONLINE SHOP<<<\n");
	while(1) {
		printf("\n");
		printf("What do you want to do?\n");
		printf("1)Show products\n");
		printf("2)Buy products\n");
		printf("3)Add products\n");
		printf("4)Exit\n");
		printf("\n");
		readFromKeyboard("Write command number:");
		
		if (!strcmp((input_buffer), "1\n")) {
			showProducts();
		} else if (!strcmp((input_buffer), "2\n")) {
			printf("What do you want to buy?\n");
			buyProduct();
		} else if (!strcmp((input_buffer), "3\n")) {
			printf("Let's add?\n");
			addProduct();
		} else if (!strcmp((input_buffer), "4\n")) {
			quit();
			sleep(1);
			break;
		} else {
			system("clear");
		}
	}
    closeApp("Bye");
    return 0;
}
