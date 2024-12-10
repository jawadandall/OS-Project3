#include "server.h"

#define DEFAULT_ROOM "Lobby"

// USE THESE LOCKS AND COUNTER TO SYNCHRONIZE
extern int numReaders;
extern pthread_mutex_t rw_lock;
extern pthread_mutex_t mutex;

extern struct node *head;

extern char *server_MOTD;

extern struct room *rooms_head;

/*
 *Main thread for each client.  Receives all messages
 *and passes the data off to the correct function.  Receives
 *a pointer to the file descriptor for the socket the thread
 *should listen on
 */

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

void *client_receive(void *ptr) {
   int client = *(int *) ptr;  // socket
  
   int received, i;
   char buffer[MAXBUFF], sbuffer[MAXBUFF];  //data buffer of 2K  
   char tmpbuf[MAXBUFF];  //data temp buffer of 1K  
   char cmd[MAXBUFF], username[20];
   char *arguments[80];

   struct node *currentUser;
    
   send(client  , server_MOTD , strlen(server_MOTD) , 0 ); // Send Welcome Message of the Day.
   send(client, "\nchat>", strlen("\nchat>"), 0);

   // Creating the guest user name
  
   sprintf(username,"guest%d", client);
   printf("Guest username created: %s\n", username);  // Print the guest username

   head = insertFirstU(head, client , username);
   

   
   // Add the GUEST to the DEFAULT ROOM (i.e. Lobby)


   while (1) {
       
      if ((received = read(client , buffer, MAXBUFF)) != 0) {
      
            buffer[received] = '\0'; 
            strcpy(cmd, buffer);  
            strcpy(sbuffer, buffer);
         
            /////////////////////////////////////////////////////
            // we got some data from a client

            // 1. Tokenize the input in buf (split it on whitespace)

            // get the first token 

             arguments[0] = strtok(cmd, delimiters);

            // walk through other tokens 

             i = 0;
             while( arguments[i] != NULL ) {
                arguments[++i] = strtok(NULL, delimiters); 
                strcpy(arguments[i-1], trimwhitespace(arguments[i-1]));
             } 

             // Arg[0] = command
             // Arg[1] = user or room
             
             /////////////////////////////////////////////////////
             // 2. Execute command: TODO


            if (strcmp(arguments[0], "join") == 0)
            {
               printf("join room: %s\n", arguments[1]);  

               // perform the operations to join room arg[1]
              
               sprintf(buffer, "\nchat>");
               send(client , buffer , strlen(buffer) , 0 ); // send back to client
            }
            else if (strcmp(arguments[0], "leave") == 0)
            {
               printf("leave room: %s\n", arguments[1]); 

               // perform the operations to leave room arg[1]

               sprintf(buffer, "\nchat>");
               send(client , buffer , strlen(buffer) , 0 ); // send back to client
            } 
            else if (strcmp(arguments[0], "connect") == 0)
            {
               printf("connect to user: %s \n", arguments[1]);

               // perform the operations to connect user with socket = client from arg[1]

               sprintf(buffer, "\nchat>");
               send(client , buffer , strlen(buffer) , 0 ); // send back to client
            }
            else if (strcmp(arguments[0], "disconnect") == 0)
            {             
               printf("disconnect from user: %s\n", arguments[1]);
               
               // perform the operations to disconnect user with socket = client from arg[1]
                
               sprintf(buffer, "\nchat>");
               send(client , buffer , strlen(buffer) , 0 ); // send back to client
            }                  

            else if (strcmp(arguments[0], "rooms") == 0) {
               printf("List all the rooms\n");
               print_all_rooms();
               send(client, buffer, strlen(buffer), 0);
            }


            else if (strcmp(arguments[0], "users") == 0)
            {
                printf("A users asked to list all the users\n");
                char *info = getAllUsersInfo(head);

                
                snprintf(buffer, sizeof(buffer), info);

                
                strcat(buffer, "\nchat>");
                send(client , buffer , strlen(buffer) , 0 ); // send back to client
            }                           
            else if (strcmp(arguments[0], "login") == 0) // or use strcasecmp if case-insensitive
            {
               // Debug: Print received command
               printf("Command received: %s\n", arguments[0]);
               fflush(stdout);

               // Check if username argument is provided
               if (arguments[1] == NULL) {
                  snprintf(buffer, sizeof(buffer), "Error: Username not provided.\n");
                  send(client, buffer, strlen(buffer), 0);
                  break; // or continue, based on your loop structure
               }

               // Safely copy the username
               strncpy(username, arguments[1], sizeof(username) - 1);
               username[sizeof(username) - 1] = '\0'; // Ensure null-termination

               // Debug: Print the username
               printf("Username set to: %s\n", username);
               fflush(stdout);

               // Prepare the response message
               snprintf(buffer, sizeof(buffer), "\nchat>");
               
               // Send the response to the client
               if (send(client, buffer, strlen(buffer), 0) < 0) {
                  perror("Send failed");
                  // Handle send error
               }

               // Optionally, perform additional actions like updating rooms or DMs
            }
            else if (strcmp(arguments[0], "help") == 0 )
            {
                sprintf(buffer, "login <username> - \"login with username\" \ncreate <room> - \"create a room\" \njoin <room> - \"join a room\" \nleave <room> - \"leave a room\" \nusers - \"list all users\" \nrooms -  \"list all rooms\" \nconnect <user> - \"connect to user\" \nexit - \"exit chat\" \n");
                send(client , buffer , strlen(buffer) , 0 ); // send back to client 
            }
            else if (strcmp(arguments[0], "connect ") == 0) 
            {
               char *target_username = arguments[1]; // Extract the username
               target_username[strcspn(target_username, "\n")] = 0; // Remove newline, if present
               if (strlen(target_username) == 0) {
                  send(client, "Usage: connect <username>\n", strlen("Usage: connect <username>\n"), 0);
                  } else {
                     connect_users(client, username, target_username); // Pass to the connection logic
                     }
            }
            else if (strcmp(arguments[0], "create") == 0) {
               char *room_name = arguments[1]; // Extract the room name
               printf("Room Name: %s\n", room_name);

               if (strlen(room_name) == 0) {
                  send(client, "Usage: create <room_name>\n", strlen("Usage: create <room_name>\n"), 0);
                  } else {
                     if (create_room_command(client, room_name)) {
                        send(client, "Room created successfully.\n", strlen("Room created successfully.\n"), 0);
                        } else {
                           send(client, "Room already exists or creation failed.\n", strlen("Room already exists or creation failed.\n"), 0);
                        }
               }
            }

            else if (strcmp(arguments[0], "exit") == 0 || strcmp(arguments[0], "logout") == 0)
            {
    
                //Remove the initiating user from all rooms and direct connections, then close the socket descriptor.
                close(client);
            }                         
            else { 
                 /////////////////////////////////////////////////////////////
                 // 3. sending a message
           
                 // send a message in the following format followed by the promt chat> to the appropriate receipients based on rooms, DMs
                 // ::[userfrom]> <message>
              
                 sprintf(tmpbuf,"\n::%s> %s\nchat>", "PUTUSERFROM", sbuffer);
                 strcpy(sbuffer, tmpbuf);
                       
                 currentUser = head;
                 while(currentUser != NULL) {
                     
                     if(client != currentUser->socket){  // dont send to yourself 
                       
                         send(currentUser->socket , sbuffer , strlen(sbuffer) , 0 ); 
                     }
                     currentUser = currentUser->next;
                 }
          
            }
 
         memset(buffer, 0, sizeof(1024));
      }
   }
   return NULL;
}

void connect_users(int client_socket, const char *initiator, const char *target_username) {
    // Find the initiator and target users
    struct node *initiator_user = findU(head, (char *)initiator);
    struct node *target_user = findU(head, (char *)target_username);

    // Check if target user exists
    if (target_user == NULL) {
        send(client_socket, "User not found.\n", strlen("User not found.\n"), 0);
        return;
    }

    // Check if the target user is already in a direct chat
    if (target_user->in_chat) {
        send(client_socket, "User is already in a direct chat.\n", strlen("User is already in a direct chat.\n"), 0);
        return;
    }

    // Prompt the target user for consent
    char prompt[100];
    snprintf(prompt, sizeof(prompt), "User %s wants to connect. Type 'accept' or 'deny':\n", initiator);
    send(target_user->socket, prompt, strlen(prompt), 0);

    // Wait for the target's response (this is a simplified blocking implementation)
    char response[20];
    recv(target_user->socket, response, sizeof(response), 0);
    response[strcspn(response, "\n")] = 0; // Strip newline

    if (strcmp(response, "accept") == 0) {
        // Notify both users
        send(initiator_user->socket, "Connection accepted. You are now in direct chat.\n",
             strlen("Connection accepted. You are now in direct chat.\n"), 0);
        send(target_user->socket, "Connection accepted. You are now in direct chat.\n",
             strlen("Connection accepted. You are now in direct chat.\n"), 0);

        // Set both users' states to indicate a direct chat
        initiator_user->in_chat = true;
        target_user->in_chat = true;

        // Handle direct communication
        direct_chat(initiator_user, target_user);
    } else {
        // Notify the initiator
        send(initiator_user->socket, "Connection request denied.\n",
             strlen("Connection request denied.\n"), 0);
    }
}

void direct_chat(struct node *user1, struct node *user2) {
    char buffer[1024];

    while (1) {
        // Receive messages from user1 and forward to user2
        int bytes_read = recv(user1->socket, buffer, sizeof(buffer), 0);
        if (bytes_read <= 0 || strcmp(buffer, "exit") == 0) break;
        send(user2->socket, buffer, bytes_read, 0);

        // Receive messages from user2 and forward to user1
        bytes_read = recv(user2->socket, buffer, sizeof(buffer), 0);
        if (bytes_read <= 0 || strcmp(buffer, "exit") == 0) break;
        send(user1->socket, buffer, bytes_read, 0);
    }

    // Notify both users and reset their states
    send(user1->socket, "You have exited the direct chat.\n",
         strlen("You have exited the direct chat.\n"), 0);
    send(user2->socket, "You have exited the direct chat.\n",
         strlen("You have exited the direct chat.\n"), 0);

    user1->in_chat = false;
    user2->in_chat = false;
}

int create_room_command(int client_socket, const char *room_name) {
    struct room *new_room = create_room(room_name);
    if (new_room == NULL) {
        // Room creation failed (already exists or other error)
        return 0;
    }

    printf("Room '%s' created by client %d\n", room_name, client_socket); // Log room creation
    return 1;
}
