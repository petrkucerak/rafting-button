#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

typedef enum status { MASTER, SLAVE } status_t;
typedef enum message_type { TIME, TIME_RTT, ACK } message_type_t;

typedef struct game {
   uint64_t time;       // auto-increment value
   uint64_t deadline;   // the 0 value means an infinite loop
   uint8_t nodes_count; // count of nodes
} game_t;

typedef struct message {
   message_type_t type;
   uint64_t content;
} message_t;

typedef struct queue {
   message_t *message;
   struct queue *next;
} queue_t;

typedef struct node {
   uint64_t time;      // local time
   status_t status;    // MASTER or SLAVE
   uint8_t time_speed; // control speed of local time
   queue_t *queue;
} node_t;

void push_to_queue(message_t* message, uint8_t node_no);
message_t* pop_from_queue(uint8_t node_no);

#endif // MAIN_H