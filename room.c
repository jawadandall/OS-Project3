#include "room.h"


struct room *rooms_head = NULL;  // Head of the room list
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER;  // Define rw_lock here

// struct room *create_room(const char *room_name) {
//     pthread_mutex_lock(&rw_lock);

//     // Check if room already exists
//     struct room *current = rooms_head;
//     while (current != NULL) {
//         if (strcmp(current->name, room_name) == 0) {
//             pthread_mutex_unlock(&rw_lock);
//             return NULL;  // Room already exists
//         }
//         current = current->next;
//     }

//     // Create a new room
//     struct room *new_room = (struct room *)malloc(sizeof(struct room));
//     if (new_room == NULL) {
//         pthread_mutex_unlock(&rw_lock);
//         perror("Failed to allocate memory for new room");
//         return NULL;
//     }

//     strcpy(new_room->name, room_name);
//     new_room->users = NULL;
//     new_room->next = rooms_head;
//     rooms_head = new_room;

//     pthread_mutex_unlock(&rw_lock);
//     return new_room;
// }

struct room *create_room(const char *room_name) {
    pthread_mutex_lock(&rw_lock);

    // Check if the room already exists
    struct room *current = rooms_head;
    while (current != NULL) {
        if (strcmp(current->name, room_name) == 0) {
            pthread_mutex_unlock(&rw_lock);
            return NULL;  // Room already exists
        }
        current = current->next;
    }

    // Allocate memory for the new room
    struct room *new_room = (struct room *)malloc(sizeof(struct room));
    if (new_room == NULL) {
        pthread_mutex_unlock(&rw_lock);
        perror("Failed to allocate memory for new room");
        return NULL;
    }

    // Initialize the room's fields
    strcpy(new_room->name, room_name);
    new_room->users = NULL;
    new_room->next = rooms_head;  // Add to the front of the linked list
    rooms_head = new_room;

    pthread_mutex_unlock(&rw_lock);

    printf("Room '%s' created successfully.\n", room_name); // Debug log
    return new_room;
}


//
void print_all_rooms() {
    pthread_mutex_lock(&rw_lock);

    struct room *current = rooms_head;
    if (current == NULL) {
        printf("No rooms available.\n");
    } else {
        printf("List of Rooms:\n");
        while (current != NULL) {
            printf(" - %s\n", current->name);
            current = current->next;
        }
    }

    pthread_mutex_unlock(&rw_lock);
}
