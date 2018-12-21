#include "product.h"

int buyProduct(char* name, int amount) {
	for (int i = 0; i < prod_list_size; i++) {
		if(!strcmp(prod_list[i].name, name)) {
			prod_list[i].amount = prod_list[i].amount  - amount;
			return 0;
		}
	}
	return -1;
	
}
