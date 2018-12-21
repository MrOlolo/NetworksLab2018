#include "product.h"

void writeProductsToFile() {
	FILE *file;
	if ((file = fopen(PRODUCTS,"w")) == NULL) {
		perror("ERROR occured while opening file");
		fclose(file);
		return;
	} 
	for (int i = 0; i < prod_list_size; i++) {
		fputs(prod_list[i].name, file);
		fprintf(file, "\n");
		fprintf(file, "%d", prod_list[i].amount);
		fprintf(file, "\n");
	}
} 
