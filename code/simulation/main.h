#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// SIMULATION TIME
// - targeted delay is between 1 ms - 10 ms
// - global time 1 step is 100 µs
// - local time (f=240MHz => T=4.16e-9) so 1 step represents 25000ticks

typedef enum status { MASTER, SLAVE } status_t;
typedef enum message_type { TIME, TIME_RTT, ACK } message_type_t;

typedef struct game {
   uint64_t time;       // auto-increment value, 1 step represents 100 µs
   uint64_t deadline;   // the 0 value means an infinite loop
   uint8_t nodes_count; // count of nodes
} game_t;

typedef struct message {
   message_type_t type;
   uint64_t content;
   uint8_t source;
} message_t;

typedef struct queue {
   message_t *message;
   struct queue *next;
} queue_t;

typedef struct node {
   uint64_t time;   // auto incement, 1 step represents 25000 procesor ticks
   status_t status; // MASTER or SLAVE
   uint8_t time_speed;
   uint32_t latency;
   queue_t *queue_head;
   queue_t *queue_tail;
   uint8_t is_first_setup;
} node_t;

void push_to_queue(message_t *message, uint8_t node_no);
message_t *pop_from_queue(uint8_t node_no);

void send_message(uint64_t content, message_type_t type, uint8_t target,
                  uint8_t source);

uint8_t is_queue_empty(uint8_t node_no);
uint8_t is_node_master(uint8_t node_no);

#endif // MAIN_H