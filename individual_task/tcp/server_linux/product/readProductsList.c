#include "product.h"

void readProductsList() {
	FILE *file;
	if ((file = fopen(PRODUCTS,"r")) == NULL) {
		perror("ERROR occured while opening file");
		fclose(file);
		return;
	} 
	
	char ch;
	int i = 0;
	int line = 0;
	char buffer[NAME_LENGTH];
	bzero(buffer, NAME_LENGTH);
	
	while((ch = getc(file)) != EOF) {
		if (ch == '\n') {
			if (line%2 == 0) {
				strcpy(prod_list[line/2].name, buffer);
			} else {
				prod_list[line/2].amount = atoi(buffer);
			}
			bzero(buffer, NAME_LENGTH);
			line++;
			i = 0;
			continue;
		}
		buffer[i++] = ch;
	}
	prod_list_size = line/2;
	fclose(file);
}

