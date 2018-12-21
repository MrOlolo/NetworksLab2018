#include "socket/sock.h"

enum  action {
	add,
	buy
};

char msg_buffer[MSG_BUF_LEN];
char input_buffer[INPUT_BUF_LEN];
int sockfd;

void closeApp(int code) {
	printf("Shutdown...\n");
	close(sockfd);
	exit(code);
}

void readFromKeyboard(char *msg) {
	printf("%s\n", msg);
	bzero(input_buffer, INPUT_BUF_LEN);
	fgets(input_buffer , INPUT_BUF_LEN - 1, stdin);
}

void showProducts() {
	bzero(msg_buffer,  MSG_BUF_LEN);
	strcat(msg_buffer  + 1,  "show\n");
	if  (sendRequest(sockfd,  msg_buffer) == -1)  {
		closeApp(1);
	}
	if (getResponse(sockfd, msg_buffer) == -1) {
		closeApp(1);
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
	sprintf(msg_buffer + 1, com, input_buffer);
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
	
	strcat(msg_buffer + 1, input_buffer);
	//printf("Takaya ditch: %s", msg_buffer + 1);
	
	if  (sendRequest(sockfd,  msg_buffer) == -1) {
		closeApp(1);
	}
	if  (getResponse(sockfd, msg_buffer) == -1) {
		closeApp(1);
	}
	checkResponse(msg_buffer,  success) ;
}

void  addProduct() {
	workWithProducts(add);
}

void buyProduct() {
	workWithProducts(buy);
}

void quit() {
	printf("Bye\n");
	bzero(msg_buffer, MSG_BUF_LEN);
	strcat(msg_buffer + 1, "quit\n");
	sendRequest(sockfd, msg_buffer);
	getResponse(sockfd,  msg_buffer);
}

int main(int argc, char *argv[]) {
	
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

	sockfd = clientCreateSock(argv[1], argv[2]);
	if (sockfd == -1) {
		printf("Connection problem\n");
		closeApp(1);
	}
	
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
    closeApp(0);
    return 0;
}
