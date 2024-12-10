#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

#define MAX_USERS_STRING_LENGTH 1024  // Define an arbitrary maximum length for the output string


//insert link at the first location
struct node* insertFirstU(struct node *head, int socket, char *username) {
    
   if(findU(head,username) == NULL) {
      printf("Added %s to linked list\n", username);
           
       //create a link
       struct node *link = (struct node*) malloc(sizeof(struct node));

       link->socket = socket;
       strcpy(link->username,username);
       
       //point it to old first node
       link->next = head;

       //point first to new first node
       head = link;
 
   }
   else
       printf("Duplicate: %s\n", username);
   return head;
}

//find a link with given user
struct node* findU(struct node *head, char* username) {

   //start from the first link
   struct node* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
    while(strcmp(current->username, username) != 0) {
	
      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
	
   //if username found, return the current Link
   return current;
}

void printAllUsers(struct node *head) {
    if (head == NULL) {
        printf("No users connected.\n");
        return;
    }

    struct node *current = head;
    printf("Connected users:\n");
    
    // Traverse the list and print each user's username
    while (current != NULL) {
        printf("Username: %s, Socket: %d\n", current->username, current->socket);
        current = current->next;
    }
}
// Function to create a string with all user info
char* getAllUsersInfo(struct node *head) {
    // Allocate memory for the string to hold the user information
    char *user_info = (char*) malloc(MAX_USERS_STRING_LENGTH * sizeof(char));
    if (user_info == NULL) {
        perror("Failed to allocate memory for user info string");
        return NULL;
    }

    // Initialize the string with an empty string
    user_info[0] = '\0';

    if (head == NULL) {
        strcat(user_info, "No users connected.\n");
        return user_info;
    }

    struct node *current = head;
    strcat(user_info, "Connected users:\n");

    // Traverse the list and append each user's info to the string
    while (current != NULL) {
        char user_line[100];  // Temporary buffer for each user line
        snprintf(user_line, sizeof(user_line), "Username: %s, Socket: %d\n", current->username, current->socket);
        strcat(user_info, user_line);
        current = current->next;
    }

    return user_info;
}



