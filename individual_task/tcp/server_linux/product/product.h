#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define WELCOME "welcome.txt"
#define PRODUCTS "products.txt" 
#define SERVER_COMMANDS "server_com.txt"

#define BUF_LEN 256
#define LIST_SIZE 30
#define NAME_LENGTH 30

struct product {
	char name[NAME_LENGTH];
	int amount;
};

struct product prod_list[LIST_SIZE];

int prod_list_size;

void readProductsList();

void productsToBuffer(char* buffer);

void addProduct(char* name, int amount);

int buyProduct(char* name, int amount);

void writeProductsToFile();
