#ifndef ROOM_H
#define ROOM_H

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct node;  // Forward declaration of `node` for rooms

// Room data structure
struct room {
    char name[50];            // Room name
    struct node *users;       // Linked list of users in the room
    struct room *next;        // Pointer to the next room
};

extern struct room *rooms_head;  // Pointer to the head of the room list
extern pthread_mutex_t rw_lock;  // Declare rw_lock as extern

// Function to create a room
struct room *create_room(const char *room_name);

#endif
