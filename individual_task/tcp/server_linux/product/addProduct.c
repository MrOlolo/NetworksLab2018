#include "product.h"

void addProduct(char* name, int amount) {
	for (int i = 0; i < prod_list_size; i++) {
		if(!strcmp(prod_list[i].name, name)) {
			prod_list[i].amount = prod_list[i].amount + amount;
			return;
		}
	}
	strcpy(prod_list[prod_list_size].name, name);
	prod_list[prod_list_size].amount = amount;
	prod_list_size++;
	
	return;
}
