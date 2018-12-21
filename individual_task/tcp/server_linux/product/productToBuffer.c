#include "product.h"

void productsToBuffer(char* buffer) {
	bzero(buffer, sizeof(buffer));
	for(int i = 0; i < prod_list_size; i++) {
		char tmp[12];
		sprintf(tmp, "%d", i + 1);
		strcat(buffer, (char*)(tmp));
		strcat(buffer, ".");
		strcat(buffer, prod_list[i].name);
		strcat(buffer, "(");
		sprintf(tmp, "%d", prod_list[i].amount);
		strcat(buffer, (char*)(tmp));
		strcat(buffer, ")\n");
	}
}
