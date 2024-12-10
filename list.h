#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct node {
   char username[30];
   int socket;
   struct node *next;
   bool in_chat; // Indicates if the user is in a direct chat
};



/////////////////// USERLIST //////////////////////////

//insert node at the first location
struct node* insertFirstU(struct node *head, int socket, char *username);

//find a node with given username
struct node* findU(struct node *head, char* username);

//Print all users in linked list to terminal
void printAllUsers(struct node *head);

char* getAllUsersInfo(struct node *head);