#include "sock.h"

void checkResponse(char *buffer, char *msg) {
	switch(buffer[1]) {
		case  OK:
			printf("%s\n", msg);
			printf("%s\n", buffer + 2);
			break;
		case  BAD_REQUEST: 
			printf("BAD REQUEST\n");
			break;
		case DISCONNECT:
			printf("DISCONNECTED\n");
			printf("%s\n", buffer + 2);
			break;
		default:
			printf("UNKNOWN ERROR\n" );
			break;
	}
}
